class Inframan < Formula
  desc "Infrastructure Manager CLI"
  homepage "https://github.com/mmycin/inframan"
  url "https://github.com/mmycin/inframan/archive/refs/tags/v0.1.0.tar.gz"
  sha256 "REPLACE_WITH_SHA256"
  license "MIT"

  depends_on "cmake" => :build

  def install
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    system "#{bin}/inframan", "--version"
  end
end
