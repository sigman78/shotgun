/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <sstream>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/Container.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>

#include "configure.h"

namespace Magnum { namespace Trade { namespace Test {

struct StbImageConverterTest: TestSuite::Tester {
    explicit StbImageConverterTest();

    void wrongFormat();
    void wrongFormatHdr();
    void wrongOutputFormat();

    /** @todo test the enum constructor somehow (needs to be not loaded through plugin manager) */

    void bmpRg();

    void hdrGrayscale();

    void jpegRgb80Percent();
    void jpegGrayscale80Percent();

    void pngRgb();
    void pngGrayscale();

    void tgaRgba();

    /* Explicitly forbid system-wide plugin dependencies */
    PluginManager::Manager<AbstractImageConverter> _converterManager{"nonexistent"};
    PluginManager::Manager<AbstractImporter> _importerManager{"nonexistent"};
};

StbImageConverterTest::StbImageConverterTest() {
    addTests({&StbImageConverterTest::wrongFormat,
              &StbImageConverterTest::wrongFormatHdr,
              &StbImageConverterTest::wrongOutputFormat,

              &StbImageConverterTest::bmpRg,

              &StbImageConverterTest::hdrGrayscale,

              &StbImageConverterTest::jpegRgb80Percent,
              &StbImageConverterTest::jpegGrayscale80Percent,

              &StbImageConverterTest::pngRgb,
              &StbImageConverterTest::pngGrayscale,

              &StbImageConverterTest::tgaRgba});

    /* Load the plugin directly from the build tree. Otherwise it's static and
       already loaded. */
    #ifdef STBIMAGECONVERTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_converterManager.load(STBIMAGECONVERTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
    /* The PngImporter is optional */
    #ifdef STBIMAGEIMPORTER_PLUGIN_FILENAME
    CORRADE_INTERNAL_ASSERT(_importerManager.load(STBIMAGEIMPORTER_PLUGIN_FILENAME) & PluginManager::LoadState::Loaded);
    #endif
}

void StbImageConverterTest::wrongFormat() {
    std::unique_ptr<AbstractImageConverter> converter = _converterManager.instantiate("StbTgaImageConverter");
    ImageView2D image{PixelFormat::RGBA32F, {}, nullptr};

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter->exportToData(image));
    CORRADE_COMPARE(out.str(), "Trade::StbImageConverter::exportToData(): PixelFormat::RGBA32F is not supported for BMP/JPEG/PNG/TGA output\n");
}

void StbImageConverterTest::wrongFormatHdr() {
    std::unique_ptr<AbstractImageConverter> converter = _converterManager.instantiate("StbHdrImageConverter");
    ImageView2D image{PixelFormat::RGB8Unorm, {}, nullptr};

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter->exportToData(image));
    CORRADE_COMPARE(out.str(), "Trade::StbImageConverter::exportToData(): PixelFormat::RGB8Unorm is not supported for HDR output\n");
}

void StbImageConverterTest::wrongOutputFormat() {
    std::unique_ptr<AbstractImageConverter> converter = _converterManager.instantiate("StbImageConverter");
    ImageView2D image{PixelFormat::RGB8Unorm, {}, nullptr};

    std::ostringstream out;
    Error redirectError{&out};

    CORRADE_VERIFY(!converter->exportToData(image));
    CORRADE_COMPARE(out.str(), "Trade::StbImageConverter::exportToData(): cannot determine output format (plugin loaded as StbImageConverter)\n");
}

namespace {
    constexpr const char OriginalRgData[] = {
        0, 0, 0, 0, 0, 0, 0, 0,

        0, 0, 0, 0, 1,  2,  3,  4,
        0, 0, 0, 0, 5,  6,  7,  8,
        0, 0, 0, 0, 9, 10, 11, 12
    };

    const ImageView2D OriginalRg{PixelStorage{}.setSkip({2, 1, 0}).setRowLength(4),
        PixelFormat::RG8Unorm, {2, 3}, OriginalRgData};

    constexpr const char ConvertedRgData[] = {
        1, 1, 1, 3, 3, 3,
        5, 5, 5, 7, 7, 7,
        9, 9, 9, 11, 11, 11
    };
}

void StbImageConverterTest::bmpRg() {
    const auto data = _converterManager.instantiate("StbBmpImageConverter")->exportToData(OriginalRg);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("StbImageImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    /* RG gets expanded to RRR */
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(),
        Containers::arrayView(ConvertedRgData), TestSuite::Compare::Container);
}

namespace {
    constexpr const Float OriginalGrayscale32FData[] = {
        1.0f, 2.0f,
        3.0f, 4.0f,
        5.0f, 6.0f
    };

    const ImageView2D OriginalGrayscale32F{PixelFormat::R32F, {2, 3}, OriginalGrayscale32FData};

    constexpr const Float ConvertedGrayscale32FData[] = {
        1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 2.0f,
        3.0f, 3.0f, 3.0f, 4.0f, 4.0f, 4.0f,
        5.0f, 5.0f, 5.0f, 6.0f, 6.0f, 6.0f
    };
}

void StbImageConverterTest::hdrGrayscale() {
    const auto data = _converterManager.instantiate("StbHdrImageConverter")->exportToData(OriginalGrayscale32F);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("StbImageImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);

    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    /* R gets converted to RRR */
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB32F);
    CORRADE_COMPARE_AS(Containers::arrayCast<Float>(converted->data()),
        Containers::arrayView(ConvertedGrayscale32FData),
        TestSuite::Compare::Container);
}

namespace {
    constexpr const char OriginalJpegRgbData[] = {
        /* Skip */
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        '\x00', '\x27', '\x48', '\x10', '\x34', '\x54',
        '\x22', '\x46', '\x60', '\x25', '\x49', '\x63',
        '\x21', '\x46', '\x63', '\x13', '\x3a', '\x59', 0, 0,

        '\x5b', '\x87', '\xae', '\x85', '\xaf', '\xd5',
        '\x94', '\xbd', '\xdd', '\x96', '\xbf', '\xdf',
        '\x91', '\xbc', '\xdf', '\x72', '\x9e', '\xc1', 0, 0,

        '\x3c', '\x71', '\xa7', '\x68', '\x9c', '\xce',
        '\x8b', '\xbb', '\xe9', '\x92', '\xc3', '\xee',
        '\x8b', '\xbe', '\xed', '\x73', '\xa7', '\xd6', 0, 0,

        '\x00', '\x34', '\x70', '\x12', '\x4a', '\x83',
        '\x35', '\x6a', '\x9e', '\x45', '\x7a', '\xac',
        '\x34', '\x6c', '\x9f', '\x1d', '\x56', '\x8b', 0, 0
    };

    const ImageView2D OriginalJpegRgb{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::RGB8Unorm, {6, 4}, OriginalJpegRgbData};

    /* Slightly different due to compression artifacts. See the 100% test for
       a threshold verification. Needs to have a bigger size otherwise the
       compression makes a total mess. */
    constexpr const char ConvertedJpegRgbData[] = {
        '\x03', '\x26', '\x4c', '\x12', '\x36', '\x5a',
        '\x22', '\x46', '\x68', '\x25', '\x49', '\x69',
        '\x1c', '\x40', '\x60', '\x11', '\x35', '\x57',

        '\x5f', '\x8a', '\xb5', '\x76', '\xa1', '\xcb',
        '\x91', '\xbd', '\xe4', '\x99', '\xc5', '\xea',
        '\x8e', '\xba', '\xdf', '\x7d', '\xa9', '\xd0',

        '\x48', '\x7a', '\xad', '\x66', '\x98', '\xc9',
        '\x88', '\xbb', '\xe8', '\x94', '\xc7', '\xf2',
        '\x88', '\xbb', '\xe6', '\x77', '\xaa', '\xd7',

        '\x00', '\x2f', '\x65', '\x17', '\x4f', '\x82',
        '\x38', '\x70', '\xa1', '\x41', '\x79', '\xa8',
        '\x32', '\x6a', '\x99', '\x21', '\x59', '\x8a'
    };
}

void StbImageConverterTest::jpegRgb80Percent() {
    std::unique_ptr<AbstractImageConverter> converter = _converterManager.instantiate("StbJpegImageConverter");
    CORRADE_COMPARE(converter->configuration().value<Float>("jpegQuality"), 0.8f);

    const auto data = converter->exportToData(OriginalJpegRgb);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("StbImageImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);
    CORRADE_COMPARE(converted->size(), Vector2i(6, 4));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedJpegRgbData),
        TestSuite::Compare::Container);
}

namespace {
    constexpr const char OriginalJpegGrayscaleData[] = {
        0, 0, 0, 0, 0, 0, 0, 0, /* Skip */

        '\x00', '\x10', '\x22', '\x25', '\x21', '\x13', 0, 0,
        '\x5b', '\x85', '\x94', '\x96', '\x91', '\x72', 0, 0,
        '\x3c', '\x68', '\x8b', '\x92', '\x8b', '\x73', 0, 0,
        '\x00', '\x12', '\x35', '\x45', '\x34', '\x1d', 0, 0
    };

    /* Slightly different due to compression artifacts. See the 100% test for
       a threshold verification. Needs to have a bigger size otherwise the
       compression makes a total mess. Also, stb_image_write expands to RGB,
       so the data are inflated. */
    constexpr const char ConvertedJpegGrayscaleData[] = {
        '\x01', '\x01', '\x01', '\x11', '\x11', '\x11',
        '\x23', '\x23', '\x23', '\x27', '\x27', '\x27',
        '\x1c', '\x1c', '\x1c', '\x11', '\x11', '\x11',

        '\x65', '\x65', '\x65', '\x7d', '\x7d', '\x7d',
        '\x97', '\x97', '\x97', '\x9d', '\x9d', '\x9d',
        '\x8e', '\x8e', '\x8e', '\x7a', '\x7a', '\x7a',

        '\x3f', '\x3f', '\x3f', '\x60', '\x60', '\x60',
        '\x85', '\x85', '\x85', '\x93', '\x93', '\x93',
        '\x88', '\x88', '\x88', '\x78', '\x78', '\x78',

        '\x00', '\x00', '\x00', '\x19', '\x19', '\x19',
        '\x3b', '\x3b', '\x3b', '\x43', '\x43', '\x43',
        '\x32', '\x32', '\x32', '\x1e', '\x1e', '\x1e'
    };

    const ImageView2D OriginalJpegGrayscale{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::R8Unorm, {6, 4}, OriginalJpegGrayscaleData};
}

void StbImageConverterTest::jpegGrayscale80Percent() {
    std::unique_ptr<AbstractImageConverter> converter = _converterManager.instantiate("StbJpegImageConverter");
    CORRADE_COMPARE(converter->configuration().value<Float>("jpegQuality"), 0.8f);

    const auto data = converter->exportToData(OriginalJpegGrayscale);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("StbImageImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);
    CORRADE_COMPARE(converted->size(), Vector2i(6, 4));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedJpegGrayscaleData),
        TestSuite::Compare::Container);
}

namespace {
    constexpr const char OriginalRgbData[] = {
        0, 0, 0, 0, 0, 0, 0, 0, /* Skip */

        1, 2, 3, 2, 3, 4, 0, 0,
        3, 4, 5, 4, 5, 6, 0, 0,
        5, 6, 7, 6, 7, 8, 0, 0
    };

    const ImageView2D OriginalRgb{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::RGB8Unorm, {2, 3}, OriginalRgbData};

    constexpr const char ConvertedRgbData[] = {
        1, 2, 3, 2, 3, 4,
        3, 4, 5, 4, 5, 6,
        5, 6, 7, 6, 7, 8
    };
}

void StbImageConverterTest::pngRgb() {
    const auto data = _converterManager.instantiate("StbPngImageConverter")->exportToData(OriginalRgb);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("StbImageImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGB8Unorm);
    CORRADE_COMPARE_AS(converted->data(),
        Containers::arrayView(ConvertedRgbData),
        TestSuite::Compare::Container);
}

namespace {
    constexpr const char OriginalGrayscaleData[] = {
        /* Skip */
        0, 0, 0, 0,

        1, 2, 0, 0,
        3, 4, 0, 0,
        5, 6, 0, 0
    };

    const ImageView2D OriginalGrayscale{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::R8Unorm, {2, 3}, OriginalGrayscaleData};

    constexpr const char ConvertedGrayscaleData[] = {
        1, 2,
        3, 4,
        5, 6
    };
}

void StbImageConverterTest::pngGrayscale() {
    const auto data = _converterManager.instantiate("StbPngImageConverter")->exportToData(OriginalGrayscale);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("PngImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::R8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedGrayscaleData),
        TestSuite::Compare::Container);
}

namespace {
    constexpr const char OriginalRgbaData[] = {
        0, 0, 0, 0, 0, 0, 0, 0, /* Skip */

        1, 2, 3, 4, 2, 3, 4, 5,
        3, 4, 5, 6, 4, 5, 6, 7,
        5, 6, 7, 8, 6, 7, 8, 9
    };

    const ImageView2D OriginalRgba{PixelStorage{}.setSkip({0, 1, 0}),
        PixelFormat::RGBA8Unorm, {2, 3}, OriginalRgbaData};

    constexpr const char ConvertedRgbaData[] = {
        1, 2, 3, 4, 2, 3, 4, 5,
        3, 4, 5, 6, 4, 5, 6, 7,
        5, 6, 7, 8, 6, 7, 8, 9
    };
}

void StbImageConverterTest::tgaRgba() {
    const auto data = _converterManager.instantiate("StbTgaImageConverter")->exportToData(OriginalRgba);
    CORRADE_VERIFY(data);

    if(_importerManager.loadState("StbImageImporter") == PluginManager::LoadState::NotFound)
        CORRADE_SKIP("StbImageImporter plugin not found, cannot test");

    std::unique_ptr<AbstractImporter> importer = _importerManager.instantiate("StbImageImporter");
    CORRADE_VERIFY(importer->openData(data));
    Containers::Optional<Trade::ImageData2D> converted = importer->image2D(0);
    CORRADE_VERIFY(converted);
    CORRADE_COMPARE(converted->size(), Vector2i(2, 3));
    CORRADE_COMPARE(converted->format(), PixelFormat::RGBA8Unorm);
    CORRADE_COMPARE_AS(converted->data(), Containers::arrayView(ConvertedRgbaData),
        TestSuite::Compare::Container);
}

}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::StbImageConverterTest)
