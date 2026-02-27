# Stage 1: Build React web UI
FROM node:20-alpine AS web-builder
WORKDIR /app/src/ui/web
COPY src/ui/web/package.json src/ui/web/package-lock.json ./
RUN npm ci
COPY src/ui/web/ ./
RUN npm run build

# Stage 2: Build C++ server
FROM ubuntu:24.04 AS server-builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    curl \
    zip \
    unzip \
    tar \
    git \
    libssl-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Install vcpkg
ENV VCPKG_ROOT=/opt/vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git "$VCPKG_ROOT" \
    && "$VCPKG_ROOT/bootstrap-vcpkg.sh" -disableMetrics

WORKDIR /app
COPY CMakeLists.txt CMakePresets.json vcpkg.json ./
COPY src/ src/
COPY third_party/ third_party/
COPY resources/ resources/
COPY lang/ lang/

RUN cmake -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DMEOS_USE_MYSQL=OFF \
    -DMEOS_BUILD_WEB=OFF

RUN cmake --build build --target MeOS-server

# Stage 3: Minimal runtime image
FROM ubuntu:24.04 AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
    libssl3t64 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN useradd --system --create-home meos
WORKDIR /opt/meos

COPY --from=server-builder /app/build/MeOS-server ./MeOS-server
COPY --from=web-builder /app/src/ui/web/dist/ ./dist/
COPY lang/ ./lang/

USER meos
EXPOSE 2009

ENTRYPOINT ["./MeOS-server"]
CMD ["--web-root", "./dist"]
