﻿/*!
 * \copy
 *     Copyright (c)  2009-2015, Cisco Systems
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions
 *     are met:
 *
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *     COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *     ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *     POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * \file    WelsThreadPool.cpp
 *
 * \brief   functions for Thread Pool
 *
 * \date    5/09/2012 Created
 *
 *************************************************************************************
 */
#include "typedefs.h"
#include "memory_align.h"
#include "WelsThreadPool.h"

namespace WelsCommon
{

int32_t CWelsThreadPool::m_iRefCount = 0;
CWelsLock CWelsThreadPool::m_cInitLock;
int32_t CWelsThreadPool::m_iMaxThreadNum = DEFAULT_THREAD_NUM;

CWelsThreadPool::CWelsThreadPool() :
    m_cWaitedTasks (NULL), m_cIdleThreads (NULL), m_cBusyThreads (NULL)
{
}


CWelsThreadPool::~CWelsThreadPool()
{
    //fprintf(stdout, "CWelsThreadPool::~CWelsThreadPool: delete %x, %x, %x\n", m_cWaitedTasks, m_cIdleThreads, m_cBusyThreads);
    if (0 != m_iRefCount)
    {
        m_iRefCount = 0;
        Uninit();
    }
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::SetThreadNum (int32_t iMaxThreadNum)
{
    CWelsAutoLock  cLock (m_cInitLock);

    if (m_iRefCount != 0)
    {
        return WELS_THREAD_ERROR_GENERAL;
    }

    if (iMaxThreadNum <= 0)
    {
        iMaxThreadNum = 1;
    }
    m_iMaxThreadNum = iMaxThreadNum;
    return WELS_THREAD_ERROR_OK;
}


CWelsThreadPool& CWelsThreadPool::AddReference ()
{
    CWelsAutoLock  cLock (m_cInitLock);
    static CWelsThreadPool m_cThreadPoolSelf;
    if (m_iRefCount == 0)
    {
        //TODO: will remove this afterwards
        if (WELS_THREAD_ERROR_OK != m_cThreadPoolSelf.Init())
        {
            m_cThreadPoolSelf.Uninit();
        }
    }

    //fprintf(stdout, "m_iRefCount=%d, pSink=%x, iMaxThreadNum=%d\n", m_iRefCount, pSink, iMaxThreadNum);

    ++ m_iRefCount;
    //fprintf(stdout, "m_iRefCount2=%d\n", m_iRefCount);
    return m_cThreadPoolSelf;
}

void CWelsThreadPool::RemoveInstance()
{
    CWelsAutoLock  cLock (m_cInitLock);
    //fprintf(stdout, "m_iRefCount=%d\n", m_iRefCount);
    -- m_iRefCount;
    if (0 == m_iRefCount)
    {
        StopAllRunning();
        Uninit();
        //fprintf(stdout, "m_iRefCount=%d, IdleThreadNum=%d, BusyThreadNum=%d, WaitedTask=%d\n", m_iRefCount, GetIdleThreadNum(), GetBusyThreadNum(), GetWaitedTaskNum());
    }
}


bool CWelsThreadPool::IsReferenced()
{
    CWelsAutoLock  cLock (m_cInitLock);
    return (m_iRefCount>0);
}


WELS_THREAD_ERROR_CODE CWelsThreadPool::OnTaskStart (CWelsTaskThread* pThread, IWelsTask* pTask)
{
    AddThreadToBusyList (pThread);
    //fprintf(stdout, "CWelsThreadPool::AddThreadToBusyList: Task %x at Thread %x\n", pTask, pThread);
    return WELS_THREAD_ERROR_OK;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::OnTaskStop (CWelsTaskThread* pThread, IWelsTask* pTask)
{
    //fprintf(stdout, "CWelsThreadPool::OnTaskStop 0: Task %x at Thread %x Finished\n", pTask, pThread);

    RemoveThreadFromBusyList (pThread);
    AddThreadToIdleQueue (pThread);
    //fprintf(stdout, "CWelsThreadPool::OnTaskStop 1: Task %x at Thread %x Finished, m_pSink=%x\n", pTask, pThread, m_pSink);

    if (pTask->GetSink())
    {
        pTask->GetSink()->OnTaskExecuted();
    }
    //if (m_pSink) {
    //  m_pSink->OnTaskExecuted (pTask);
    //}
    //fprintf(stdout, "CWelsThreadPool::OnTaskStop 2: Task %x at Thread %x Finished\n", pTask, pThread);

    SignalThread();

    //fprintf(stdout, "ThreadPool: Task %x at Thread %x Finished\n", pTask, pThread);
    return WELS_THREAD_ERROR_OK;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::Init ()
{
    //fprintf(stdout, "Enter WelsThreadPool Init\n");

    CWelsAutoLock  cLock (m_cLockPool);

    m_cWaitedTasks = new CWelsCircleQueue<IWelsTask>();
    m_cIdleThreads = new CWelsCircleQueue<CWelsTaskThread>();
    m_cBusyThreads = new CWelsList<CWelsTaskThread>();
    if (NULL == m_cWaitedTasks || NULL == m_cIdleThreads || NULL == m_cBusyThreads)
    {
        return WELS_THREAD_ERROR_GENERAL;
    }

    for (int32_t i = 0; i < m_iMaxThreadNum; i++)
    {
        if (WELS_THREAD_ERROR_OK != CreateIdleThread())
        {
            return WELS_THREAD_ERROR_GENERAL;
        }
    }

    if (WELS_THREAD_ERROR_OK != Start())
    {
        return WELS_THREAD_ERROR_GENERAL;
    }

    return WELS_THREAD_ERROR_OK;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::StopAllRunning()
{
    WELS_THREAD_ERROR_CODE iReturn = WELS_THREAD_ERROR_OK;

    ClearWaitedTasks();

    while (GetBusyThreadNum() > 0)
    {
        //WELS_INFO_TRACE ("CWelsThreadPool::Uninit - Waiting all thread to exit");
        WelsSleep (10);
    }

    if (GetIdleThreadNum() != m_iMaxThreadNum)
    {
        iReturn = WELS_THREAD_ERROR_GENERAL;
    }

    return iReturn;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::Uninit()
{
    WELS_THREAD_ERROR_CODE iReturn = WELS_THREAD_ERROR_OK;
    CWelsAutoLock  cLock (m_cLockPool);

    iReturn = StopAllRunning();
    if (WELS_THREAD_ERROR_OK != iReturn)
    {
        return iReturn;
    }

    m_cLockIdleTasks.Lock();
    while (m_cIdleThreads->size() > 0)
    {
        DestroyThread (m_cIdleThreads->begin());
        m_cIdleThreads->pop_front();
    }
    m_cLockIdleTasks.Unlock();

    Kill();

    WELS_DELETE_OP(m_cWaitedTasks);
    WELS_DELETE_OP(m_cIdleThreads);
    WELS_DELETE_OP(m_cBusyThreads);

    return iReturn;
}

void CWelsThreadPool::ExecuteTask()
{
    //fprintf(stdout, "ThreadPool: scheduled tasks: ExecuteTask\n");
    CWelsTaskThread* pThread = NULL;
    IWelsTask*    pTask = NULL;
    while (GetWaitedTaskNum() > 0)
    {
        pThread = GetIdleThread();
        if (pThread == NULL)
        {
            break;
        }
        pTask = GetWaitedTask();
        //fprintf(stdout, "ThreadPool:  ExecuteTask = %x at thread %x\n", pTask, pThread);
        pThread->SetTask (pTask);
    }
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::QueueTask (IWelsTask* pTask)
{
    CWelsAutoLock  cLock (m_cLockPool);

    //fprintf(stdout, "CWelsThreadPool::QueueTask: %d, pTask=%x\n", m_iRefCount, pTask);
    if (GetWaitedTaskNum() == 0)
    {
        CWelsTaskThread* pThread = GetIdleThread();

        if (pThread != NULL)
        {
            //fprintf(stdout, "ThreadPool:  ExecuteTask = %x at thread %x\n", pTask, pThread);
            pThread->SetTask (pTask);

            return WELS_THREAD_ERROR_OK;
        }
    }
    //fprintf(stdout, "ThreadPool:  AddTaskToWaitedList: %x\n", pTask);
    if (false == AddTaskToWaitedList (pTask))
    {
        return WELS_THREAD_ERROR_GENERAL;
    }

    //fprintf(stdout, "ThreadPool:  SignalThread: %x\n", pTask);
    SignalThread();
    return WELS_THREAD_ERROR_OK;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::CreateIdleThread()
{
    CWelsTaskThread* pThread = new CWelsTaskThread (this);

    if (NULL == pThread)
    {
        return WELS_THREAD_ERROR_GENERAL;
    }

    if (WELS_THREAD_ERROR_OK != pThread->Start())
    {
        return WELS_THREAD_ERROR_GENERAL;
    }
    //fprintf(stdout, "ThreadPool:  AddThreadToIdleQueue: %x\n", pThread);
    AddThreadToIdleQueue (pThread);

    return WELS_THREAD_ERROR_OK;
}

void  CWelsThreadPool::DestroyThread (CWelsTaskThread* pThread)
{
    pThread->Kill();
    WELS_DELETE_OP(pThread);

    return;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::AddThreadToIdleQueue (CWelsTaskThread* pThread)
{
    CWelsAutoLock cLock (m_cLockIdleTasks);
    m_cIdleThreads->push_back (pThread);
    return WELS_THREAD_ERROR_OK;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::AddThreadToBusyList (CWelsTaskThread* pThread)
{
    CWelsAutoLock cLock (m_cLockBusyTasks);
    m_cBusyThreads->push_back (pThread);
    return WELS_THREAD_ERROR_OK;
}

WELS_THREAD_ERROR_CODE CWelsThreadPool::RemoveThreadFromBusyList (CWelsTaskThread* pThread)
{
    CWelsAutoLock cLock (m_cLockBusyTasks);
    if (m_cBusyThreads->erase (pThread))
    {
        return WELS_THREAD_ERROR_OK;
    }
    else
    {
        return WELS_THREAD_ERROR_GENERAL;
    }
}

bool  CWelsThreadPool::AddTaskToWaitedList (IWelsTask* pTask)
{
    CWelsAutoLock  cLock (m_cLockWaitedTasks);

    int32_t nRet = m_cWaitedTasks->push_back (pTask);
    //fprintf(stdout, "CWelsThreadPool::AddTaskToWaitedList=%d, pTask=%x\n", m_cWaitedTasks->size(), pTask);
    return (0==nRet ? true : false);
}

CWelsTaskThread*   CWelsThreadPool::GetIdleThread()
{
    CWelsAutoLock cLock (m_cLockIdleTasks);

    //fprintf(stdout, "CWelsThreadPool::GetIdleThread=%d\n", m_cIdleThreads->size());
    if (m_cIdleThreads->size() == 0)
    {
        return NULL;
    }

    CWelsTaskThread* pThread = m_cIdleThreads->begin();
    m_cIdleThreads->pop_front();
    return pThread;
}

int32_t  CWelsThreadPool::GetBusyThreadNum()
{
    return m_cBusyThreads->size();
}

int32_t  CWelsThreadPool::GetIdleThreadNum()
{
    return m_cIdleThreads->size();
}

int32_t  CWelsThreadPool::GetWaitedTaskNum()
{
    //fprintf(stdout, "CWelsThreadPool::m_cWaitedTasks=%d\n", m_cWaitedTasks->size());
    return m_cWaitedTasks->size();
}

IWelsTask* CWelsThreadPool::GetWaitedTask()
{
    CWelsAutoLock lock (m_cLockWaitedTasks);

    if (m_cWaitedTasks->size() == 0)
    {
        return NULL;
    }

    IWelsTask* pTask = m_cWaitedTasks->begin();

    m_cWaitedTasks->pop_front();

    return pTask;
}

void  CWelsThreadPool::ClearWaitedTasks()
{
    CWelsAutoLock cLock (m_cLockWaitedTasks);
    IWelsTask* pTask = NULL;
    while (0 != m_cWaitedTasks->size())
    {
        pTask = m_cWaitedTasks->begin();
        if (pTask->GetSink())
        {
            pTask->GetSink()->OnTaskCancelled();
        }
        m_cWaitedTasks->pop_front();
    }
}

}


