#!/bin/zsh

# Starts up the docker linux subsystem using the Dockerfile

open /Applications/Docker.app

result=$(docker ps -a | grep clion_linux_env)

if [[ -n "$result" ]]; then
  # The container exists
  echo "Docker container exists, using it..."
  docker start clion_linux_env
else
  echo "Docker container does not exist, creating one..."
  # Build a new docker container
  docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name clion_linux_env clion/remote-ubuntu:0.5
fi