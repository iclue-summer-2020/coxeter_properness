FROM ubuntu:16.04

WORKDIR /iclue-summer-2020/coxeter-properness

RUN apt-get -y update    \
 && apt-get -y install   \
      bash               \
      build-essential    \
      git                \
      python3            \
      python3-dev        \
      python3-pip        \
      time               \
 && python3 -m pip install --upgrade pip==20.3.4 \
 && python3 -m pip install   \
      cmake==3.17.3

COPY . .
