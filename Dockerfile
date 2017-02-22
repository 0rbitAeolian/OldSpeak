FROM ubuntu:14.04

ENV DEBIAN_FRONTEND  noninteractive
ENV PYTHONUNBUFFERED true
ENV VIRTUAL_ENV      /mnt/oldspeak/venv
ENV PATH             $VIRTUAL_ENV/bin:$PATH
ENV DEPS_PATH        /mnt/oldspeak/deps

MAINTAINER th0ughtcr1me@protonmail.ch

# install system dependencies
RUN apt-get update \
  && apt-get --yes install \
    bash \
    build-essential \
    ca-certificates \
    cmake \
    coreutils \
    curl \
    git \
    git-core \
    gnupg2 \
    libev-dev \
    libffi-dev \
    libssl-dev \
    libudev-dev \
    pkg-config \
    python-pip \
    python-virtualenv \
    python2.7 \
    python2.7-dev \
    rsync \
    tree \
    vim \
    wget \
  && rm -rf /var/lib/apt/lists/*

RUN rm -f /bin/sh && ln -s /bin/bash /bin/sh
RUN mkdir -p /mnt/oldspeak/{src,venv}
RUN mkdir -p $DEPS_PATH


# oooo   o8o   .o8                   o8o      .     .oooo.
# `888   `"'  "888                   `"'    .o8   .dP""Y88b
#  888  oooo   888oooo.   .oooooooo oooo  .o888oo       ]8P'
#  888  `888   d88' `88b 888' `88b  `888    888       .d8P'
#  888   888   888   888 888   888   888    888     .dP'
#  888   888   888   888 `88bod8P'   888    888 . .oP     .o
# o888o o888o  `Y8bod8P' `8oooooo.  o888o   "888" 8888888888
#                        d"     YD
#                        "Y88888P'

ENV LIBGIT2_VERSION "0.25.0"
ENV LIBGIT2_TARNAME v${LIBGIT2_VERSION}.tar.gz
ENV LIBGIT2_PACKAGE ${DEPS_PATH}/${LIBGIT2_TARNAME}
ENV LIBGIT2_PATH    ${DEPS_PATH}/libgit2-${LIBGIT2_VERSION}
ENV LIBGIT2_URL     https://github.com/libgit2/libgit2/archive/${LIBGIT2_TARNAME}

RUN (test -f ${LIBGIT2_PACKAGE} || wget ${LIBGIT2_URL} -O ${LIBGIT2_PACKAGE}) \
    && (test -d ${LIBGIT2_PATH} || tar xzf ${LIBGIT2_PACKAGE} -C ${DEPS_PATH}) \
    && (cd ${LIBGIT2_PATH} && cmake .) \
    && (cd ${LIBGIT2_PATH} && make) \
    && (cd ${LIBGIT2_PATH} && make install)

#  .oooooooo oo.ooooo.   .oooooooo ooo. .oo.  .oo.    .ooooo.
# 888' `88b   888' `88b 888' `88b  `888P"Y88bP"Y88b  d88' `88b
# 888   888   888   888 888   888   888   888   888  888ooo888
# `88bod8P'   888   888 `88bod8P'   888   888   888  888    .o
# `8oooooo.   888bod8P' `8oooooo.  o888o o888o o888o `Y8bod8P'
# d"     YD   888       d"     YD
# "Y88888P'  o888o      "Y88888P'


ENV LIBGPGERROR_VERSION  "1.26"
ENV LIBGPGERROR_TARNAME  libgpg-error-${LIBGPGERROR_VERSION}.tar.bz2
ENV LIBGPGERROR_PACKAGE  ${DEPS_PATH}/${LIBGPGERROR_TARNAME}
ENV LIBGPGERROR_PATH     ${DEPS_PATH}/libgpg-error-${LIBGPGERROR_VERSION}
ENV LIBGPGERROR_URL      https://www.gnupg.org/ftp/gcrypt/libgpg-error/${LIBGPGERROR_TARNAME}

ENV LIBKSBA_VERSION  "1.3.5"
ENV LIBKSBA_TARNAME  libksba-${LIBKSBA_VERSION}.tar.bz2
ENV LIBKSBA_PACKAGE  ${DEPS_PATH}/${LIBKSBA_TARNAME}
ENV LIBKSBA_PATH     ${DEPS_PATH}/libksba-${LIBKSBA_VERSION}
ENV LIBKSBA_URL      https://www.gnupg.org/ftp/gcrypt/libksba/${LIBKSBA_TARNAME}

ENV LIBASSUAN_VERSION  "2.4.3"
ENV LIBASSUAN_TARNAME  libassuan-${LIBASSUAN_VERSION}.tar.bz2
ENV LIBASSUAN_PACKAGE  ${DEPS_PATH}/${LIBASSUAN_TARNAME}
ENV LIBASSUAN_PATH     ${DEPS_PATH}/libassuan-${LIBASSUAN_VERSION}
ENV LIBASSUAN_URL      https://www.gnupg.org/ftp/gcrypt/libassuan/${LIBASSUAN_TARNAME}

ENV LIBGCRYPT_VERSION  "1.7.6"
ENV LIBGCRYPT_TARNAME  libgcrypt-${LIBGCRYPT_VERSION}.tar.bz2
ENV LIBGCRYPT_PACKAGE  ${DEPS_PATH}/${LIBGCRYPT_TARNAME}
ENV LIBGCRYPT_PATH     ${DEPS_PATH}/libgcrypt-${LIBGCRYPT_VERSION}
ENV LIBGCRYPT_URL      https://www.gnupg.org/ftp/gcrypt/libgcrypt/${LIBGCRYPT_TARNAME}

ENV LIBGPGME_VERSION     "1.8.0"
ENV LIBGPGME_TARNAME  gpgme-${LIBGPGME_VERSION}.tar.bz2
ENV LIBGPGME_PACKAGE ${DEPS_PATH}/${LIBGPGME_TARNAME}
ENV LIBGPGME_PATH   ${DEPS_PATH}/gpgme-${LIBGPGME_VERSION}
ENV LIBGPGME_URL    https://www.gnupg.org/ftp/gcrypt/gpgme/${LIBGPGME_TARNAME}


RUN (test -f ${LIBGPGERROR_PACKAGE} || wget ${LIBGPGERROR_URL} -O ${LIBGPGERROR_PACKAGE}) \
    && (test -d ${LIBGPGERROR_PATH} || tar xjf ${LIBGPGERROR_PACKAGE} -C ${DEPS_PATH}) \
    && (cd ${LIBGPGERROR_PATH} && ./configure --prefix=/usr) \
    && (cd ${LIBGPGERROR_PATH} && make) \
    && (cd ${LIBGPGERROR_PATH} && make install)

RUN (test -f ${LIBKSBA_PACKAGE} || wget ${LIBKSBA_URL} -O ${LIBKSBA_PACKAGE}) \
    && (test -d ${LIBKSBA_PATH} || tar xjf ${LIBKSBA_PACKAGE} -C ${DEPS_PATH}) \
    && (cd ${LIBKSBA_PATH} && ./configure --prefix=/usr) \
    && (cd ${LIBKSBA_PATH} && make) \
    && (cd ${LIBKSBA_PATH} && make install)

RUN (test -f ${LIBASSUAN_PACKAGE} || wget ${LIBASSUAN_URL} -O ${LIBASSUAN_PACKAGE}) \
    && (test -d ${LIBASSUAN_PATH} || tar xjf ${LIBASSUAN_PACKAGE} -C ${DEPS_PATH}) \
    && (cd ${LIBASSUAN_PATH} && ./configure --prefix=/usr) \
    && (cd ${LIBASSUAN_PATH} && make) \
    && (cd ${LIBASSUAN_PATH} && make install)

RUN (test -f ${LIBGCRYPT_PACKAGE} || wget ${LIBGCRYPT_URL} -O ${LIBGCRYPT_PACKAGE}) \
    && (test -d ${LIBGCRYPT_PATH} || tar xjf ${LIBGCRYPT_PACKAGE} -C ${DEPS_PATH}) \
    && (cd ${LIBGCRYPT_PATH} && ./configure --prefix=/usr) \
    && (cd ${LIBGCRYPT_PATH} && make) \
    && (cd ${LIBGCRYPT_PATH} && make install)

RUN (test -f ${LIBGPGME_PACKAGE} || wget ${LIBGPGME_URL} -O ${LIBGPGME_PACKAGE}) \
    && (test -d ${LIBGPGME_PATH} || tar xjf ${LIBGPGME_PACKAGE} -C ${DEPS_PATH}) \
    && (cd ${LIBGPGME_PATH} && ./configure --prefix=/usr) \
    && (cd ${LIBGPGME_PATH} && make) \
    && (cd ${LIBGPGME_PATH} && make install)

ENV GPATHZ                "${LIBASSUAN_PATH}:${LIBKSBA_PATH}:${LIBGCRYPT_PATH}:${LIBGPGME_PATH}:${LIBGPGERROR_PATH}:${LIBGIT2_PATH}"
ENV C_INCLUDE_PATH        "${C_INCLUDE_PATH}:${GPATHZ}"
ENV LD_LIBRARY_PATH       "${LD_LIBRARY_PATH}:${GPATHZ}"
ENV CPLUS_INCLUDE_PATH    "${CPLUS_INCLUDE_PATH}:${GPATHZ}"

# install core python dependencies
RUN pip install -U pip virtualenv

COPY . $DEPS_PATH
WORKDIR $DEPS_PATH

RUN pip install -r development.txt
RUN make clean
RUN make build
RUN make tests
