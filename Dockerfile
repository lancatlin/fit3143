FROM ubuntu:26.04

RUN apt-get update && apt-get install -y \
  build-essential \
  libomp-dev \
  openmpi-bin \
  libopenmpi-dev \
  libprrte-bin \
  libprrte-dev \
  openssh-server \
  git \
  vim \
  && rm -rf /var/lib/apt/lists/*

RUN ssh-keygen -A

RUN usermod -l student -d /home/student -m ubuntu \
  && groupmod -n student ubuntu

RUN mkdir -p /var/run/sshd /home/student/.ssh \
  && chmod 700 /home/student/.ssh \
  && chown -R student:student /home/student/.ssh

COPY ./ssh/authorized_keys /home/student/.ssh/authorized_keys

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
