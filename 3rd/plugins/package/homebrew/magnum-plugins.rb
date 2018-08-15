# kate: indent-width 2;

class MagnumPlugins < Formula
  desc "Plugins for the Magnum C++11/C++14 graphics engine"
  homepage "http://magnum.graphics"
  url "https://github.com/mosra/magnum-plugins/archive/v2018.02.tar.gz"
  sha256 "6f4523d2076c24a0d6eddc02aadc5bae4b48d57215882c2335eec44a1559e66d"
  head "git://github.com/mosra/magnum-plugins.git"

  depends_on "assimp"
  depends_on "cmake"
  depends_on "magnum"
  depends_on "devil"
  depends_on "freetype"
  depends_on "harfbuzz"
  depends_on "libpng"
  depends_on "jpeg"

  def install
    system "mkdir build"
    cd "build" do
      system "cmake", "-DCMAKE_BUILD_TYPE=Release", "-DCMAKE_INSTALL_PREFIX=#{prefix}", "-DWITH_ASSIMPIMPORTER=ON", "-DWITH_DDSIMPORTER=ON", "-DWITH_DEVILIMAGEIMPORTER=ON", "-DWITH_DRFLACAUDIOIMPORTER=ON", "-DWITH_DRWAVAUDIOIMPORTER=ON", "-DWITH_FREETYPEFONT=ON", "-DWITH_HARFBUZZFONT=ON", "-DWITH_JPEGIMAGECONVERTER=ON", "-DWITH_JPEGIMPORTER=ON", "-DWITH_MINIEXRIMAGECONVERTER=ON", "-DWITH_OPENGEXIMPORTER=ON", "-DWITH_PNGIMAGECONVERTER=ON", "-DWITH_PNGIMPORTER=ON", "-DWITH_STANFORDIMPORTER=ON", "-DWITH_STBIMAGECONVERTER=ON", "-DWITH_STBIMAGEIMPORTER=ON", "-DWITH_STBTRUETYPEFONT=ON", "-DWITH_STBVORBISAUDIOIMPORTER=ON", "-DWITH_TINYGLTFIMPORTER=ON", ".."
      system "cmake", "--build", "."
      system "cmake", "--build", ".", "--target", "install"
    end
  end
end
