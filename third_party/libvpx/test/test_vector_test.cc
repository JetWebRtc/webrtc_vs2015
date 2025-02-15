﻿/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>
#include "third_party/googletest/src/include/gtest/gtest.h"
#include "../tools_common.h"
#include "./vpx_config.h"
#include "test/codec_factory.h"
#include "test/decode_test_driver.h"
#include "test/ivf_video_source.h"
#include "test/md5_helper.h"
#include "test/test_vectors.h"
#include "test/util.h"
#if CONFIG_WEBM_IO
#include "test/webm_video_source.h"
#endif
#include "vpx_mem/vpx_mem.h"

namespace
{

enum DecodeMode { kSerialMode, kFrameParallelMode };

const int kDecodeMode = 0;
const int kThreads = 1;
const int kFileName = 2;

typedef std::tr1::tuple<int, int, const char *> DecodeParam;

class TestVectorTest : public ::libvpx_test::DecoderTest,
    public ::libvpx_test::CodecTestWithParam<DecodeParam>
{
protected:
    TestVectorTest() : DecoderTest(GET_PARAM(0)), md5_file_(NULL)
    {
#if CONFIG_VP9_DECODER
        resize_clips_.insert(::libvpx_test::kVP9TestVectorsResize,
                             ::libvpx_test::kVP9TestVectorsResize +
                             ::libvpx_test::kNumVP9TestVectorsResize);
#endif
    }

    virtual ~TestVectorTest()
    {
        if (md5_file_) fclose(md5_file_);
    }

    void OpenMD5File(const std::string &md5_file_name_)
    {
        md5_file_ = libvpx_test::OpenTestDataFile(md5_file_name_);
        ASSERT_TRUE(md5_file_ != NULL) << "Md5 file open failed. Filename: "
                                       << md5_file_name_;
    }

    virtual void DecompressedFrameHook(const vpx_image_t &img,
                                       const unsigned int frame_number)
    {
        ASSERT_TRUE(md5_file_ != NULL);
        char expected_md5[33];
        char junk[128];

        // Read correct md5 checksums.
        const int res = fscanf(md5_file_, "%s  %s", expected_md5, junk);
        ASSERT_NE(res, EOF) << "Read md5 data failed";
        expected_md5[32] = '\0';

        ::libvpx_test::MD5 md5_res;
        md5_res.Add(&img);
        const char *actual_md5 = md5_res.Get();

        // Check md5 match.
        ASSERT_STREQ(expected_md5, actual_md5)
                << "Md5 checksums don't match: frame number = " << frame_number;
    }

#if CONFIG_VP9_DECODER
    std::set<std::string> resize_clips_;
#endif

private:
    FILE *md5_file_;
};

// This test runs through the whole set of test vectors, and decodes them.
// The md5 checksums are computed for each frame in the video file. If md5
// checksums match the correct md5 data, then the test is passed. Otherwise,
// the test failed.
TEST_P(TestVectorTest, MD5Match)
{
    const DecodeParam input = GET_PARAM(1);
    const std::string filename = std::tr1::get<kFileName>(input);
    const int threads = std::tr1::get<kThreads>(input);
    const int mode = std::tr1::get<kDecodeMode>(input);
    vpx_codec_flags_t flags = 0;
    vpx_codec_dec_cfg_t cfg = vpx_codec_dec_cfg_t();
    char str[256];

    if (mode == kFrameParallelMode)
    {
        flags |= VPX_CODEC_USE_FRAME_THREADING;
#if CONFIG_VP9_DECODER
        // TODO(hkuang): Fix frame parallel decode bug. See issue 1086.
        if (resize_clips_.find(filename) != resize_clips_.end())
        {
            printf("Skipping the test file: %s, due to frame parallel decode bug.\n",
                   filename.c_str());
            return;
        }
#endif
    }

    cfg.threads = threads;

    snprintf(str, sizeof(str) / sizeof(str[0]) - 1,
             "file: %s  mode: %s threads: %d", filename.c_str(),
             mode == 0 ? "Serial" : "Parallel", threads);
    SCOPED_TRACE(str);

    // Open compressed video file.
    testing::internal::scoped_ptr<libvpx_test::CompressedVideoSource> video;
    if (filename.substr(filename.length() - 3, 3) == "ivf")
    {
        video.reset(new libvpx_test::IVFVideoSource(filename));
    }
    else if (filename.substr(filename.length() - 4, 4) == "webm")
    {
#if CONFIG_WEBM_IO
        video.reset(new libvpx_test::WebMVideoSource(filename));
#else
        fprintf(stderr, "WebM IO is disabled, skipping test vector %s\n",
                filename.c_str());
        return;
#endif
    }
    ASSERT_TRUE(video.get() != NULL);
    video->Init();

    // Construct md5 file name.
    const std::string md5_filename = filename + ".md5";
    OpenMD5File(md5_filename);

    // Set decode config and flags.
    set_cfg(cfg);
    set_flags(flags);

    // Decode frame, and check the md5 matching.
    ASSERT_NO_FATAL_FAILURE(RunLoop(video.get(), cfg));
}

// Test VP8 decode in serial mode with single thread.
// NOTE: VP8 only support serial mode.
#if CONFIG_VP8_DECODER
VP8_INSTANTIATE_TEST_CASE(
    TestVectorTest,
    ::testing::Combine(
        ::testing::Values(0),  // Serial Mode.
        ::testing::Values(1),  // Single thread.
        ::testing::ValuesIn(libvpx_test::kVP8TestVectors,
                            libvpx_test::kVP8TestVectors +
                            libvpx_test::kNumVP8TestVectors)));

// Test VP8 decode in with different numbers of threads.
INSTANTIATE_TEST_CASE_P(
    VP8MultiThreaded, TestVectorTest,
    ::testing::Combine(
        ::testing::Values(
            static_cast<const libvpx_test::CodecFactory *>(&libvpx_test::kVP8)),
        ::testing::Combine(
            ::testing::Values(0),    // Serial Mode.
            ::testing::Range(1, 8),  // With 1 ~ 8 threads.
            ::testing::ValuesIn(libvpx_test::kVP8TestVectors,
                                libvpx_test::kVP8TestVectors +
                                libvpx_test::kNumVP8TestVectors))));

#endif  // CONFIG_VP8_DECODER

// Test VP9 decode in serial mode with single thread.
#if CONFIG_VP9_DECODER
VP9_INSTANTIATE_TEST_CASE(
    TestVectorTest,
    ::testing::Combine(
        ::testing::Values(0),  // Serial Mode.
        ::testing::Values(1),  // Single thread.
        ::testing::ValuesIn(libvpx_test::kVP9TestVectors,
                            libvpx_test::kVP9TestVectors +
                            libvpx_test::kNumVP9TestVectors)));

// Test VP9 decode in frame parallel mode with different number of threads.
INSTANTIATE_TEST_CASE_P(
    VP9MultiThreadedFrameParallel, TestVectorTest,
    ::testing::Combine(
        ::testing::Values(
            static_cast<const libvpx_test::CodecFactory *>(&libvpx_test::kVP9)),
        ::testing::Combine(
            ::testing::Values(1),    // Frame Parallel mode.
            ::testing::Range(2, 9),  // With 2 ~ 8 threads.
            ::testing::ValuesIn(libvpx_test::kVP9TestVectors,
                                libvpx_test::kVP9TestVectors +
                                libvpx_test::kNumVP9TestVectors))));
#endif
}  // namespace
