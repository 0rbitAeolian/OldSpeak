FROM ubuntu:14.04

ENV DEBIAN_FRONTEND  noninteractive
ENV PYTHONUNBUFFERED true
ENV VIRTUAL_ENV      /srv/venv
ENV PATH             $VIRTUAL_ENV/bin:$PATH

MAINTAINER gabriel@nacaolivre.org

# install system dependencies
RUN apt-get update \
  && apt-get --yes install \
    build-essential \
    ca-certificates \
    coreutils \
    python-pip \
    python2.7 \
    python2.7-dev \
    git-core \
    bash \
  && rm -rf /var/lib/apt/lists/*

# ensure that our shell is bash :)
RUN rm /bin/sh && ln -s /bin/bash /bin/sh

# install core python dependencies
RUN pip install -U pip virtualenv

# create deployment environment
RUN mkdir -p /srv/{src,venv}
RUN virtualenv /srv/venv
#RUN source /srv/venv/bin/activate
COPY . /srv/src/

WORKDIR /srv/src
RUN pip install -r development.txt
CMD ["make", "clean", "setup", "tests"]
