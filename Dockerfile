# Use prototype-base as base image, build and install dependencies only if not cached
FROM prototype-base:latest AS builder

# Install dependencies only if not cached
RUN apt-get update && apt-get install -y \
    build-essential \
    libpoppler-cpp-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
COPY . /app

WORKDIR /app

# Build the application using g++
RUN g++ -std=c++17 -I/usr/include/poppler/cpp -o main \
    src/main.cpp src/outline_extractor.cpp src/utils/file_utils.cpp src/utils/logger.cpp src/utils/json_utils.cpp src/task1a.cpp src/task1b.cpp \
    -lpoppler-cpp -lpoppler

# Final image
FROM prototype-base:latest

COPY --from=builder /app/main /app/main

WORKDIR /app

CMD ["./main"]
