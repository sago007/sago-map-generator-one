#/bin/bash
set -e

docker build -f extra/docker/Dockerfile.Ubuntu1404 . -t sago-map-generator-test
docker build -f extra/docker/Dockerfile.WindowsBuild . -t sago-map-generator-test
