VERSION 0.7

workspace:
  FROM DOCKERFILE -f .devcontainer/Dockerfile .
  WORKDIR /workspace/dao

  COPY --dir apps apps
  COPY --dir cmake cmake
  COPY --dir examples examples
  COPY --dir include include
  COPY --dir profiles profiles
  COPY --dir packages packages
  COPY CMakeLists.txt CMakeLists.txt
  COPY CMakeUserPresets.json CMakeUserPresets.json
  COPY conanfile.py conanfile.py
  COPY conan.lock conan.lock
  COPY Taskfile.yaml Taskfile.yaml

  RUN conan profile detect

deps:
  FROM +workspace

  RUN task install

build:
  FROM +deps

  RUN task generate
  RUN task build

test:
  FROM +build

  RUN task test
