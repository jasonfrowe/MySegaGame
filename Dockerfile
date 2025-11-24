# Dockerfile for SGDK with CMake support
FROM ghcr.io/stephane-d/sgdk:latest

# Switch to root to install packages
USER root

# Install CMake and other build tools
RUN apk add --no-cache cmake make

# Switch back to sgdk user
USER sgdk

# Set working directory
WORKDIR /project

# Default command
ENTRYPOINT ["/bin/sh"]
