FROM mcr.microsoft.com/vscode/devcontainers/cpp:jammy

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && apt-get dist-upgrade -y

# Install development tools
RUN apt-get install -y \
  --no-install-recommends \
  software-properties-common \
  ninja-build \
  gdb \
  valgrind \
  python3-pip

# Install Clang 16
RUN wget -nv -O - https://apt.llvm.org/llvm-snapshot.gpg.key \
  | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc \
  && add-apt-repository \
  "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-16 main" \
  && apt-get update
RUN apt-get install -y \
  --no-install-recommends \
  clang-16 \
  lld-16 \
  libc++-16-dev \
  libc++abi-16-dev \
  clangd-16 \
  clang-format-16 \
  clang-tidy-16 \
  libllvm16 \
  llvm-16 \
  llvm-16-dev

RUN apt-get clean && rm -rf "/var/lib/apt/lists/*"

# Install CMake version that supports presets
ENV CMAKE_TAG="v3.27"
ENV CMAKE_VERSION="3.27.6"
ENV CMAKE_PLATFORM="linux-x86_64"
ENV CMAKE_RELEASE="cmake-${CMAKE_VERSION}-${CMAKE_PLATFORM}"
RUN wget -qO- "https://cmake.org/files/${CMAKE_TAG}/${CMAKE_RELEASE}.tar.gz" \
  | tar --strip-components=1 -xz -C "/usr/local"

ENV CCACHE_TAG="v4.8.3"
ENV CCACHE_VERSION="4.8.3"
ENV CCACHE_PLATFORM="linux-x86_64"
ENV CCACHE_RELEASE="ccache-${CCACHE_VERSION}-${CCACHE_PLATFORM}"
RUN wget -qO- "https://github.com/ccache/ccache/releases/download/${CCACHE_TAG}/${CCACHE_RELEASE}.tar.xz" \
  | tar -xJ -C "/opt" \
  && mv "/opt/${CCACHE_RELEASE}/ccache" "/usr/local/bin" \
  && rm -rf "/opt/${CCACHE_RELEASE}"

# Install Conan package manager
RUN python3.10 -m pip install conan --upgrade

# Taskfile setup
RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b "/usr/local/bin"
