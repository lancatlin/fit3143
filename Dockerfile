FROM monashfit/fit3143

USER root
RUN apt-get update && apt-get install -y openssh-server

RUN mkdir -p /var/run/sshd /home/student/.ssh \
  && chmod 700 /home/student/.ssh \
  && chown -R student:student /home/student/.ssh

COPY ./ssh/authorized_keys /home/student/.ssh/authorized_keys

RUN ssh-keygen -A

RUN apt-get update && apt-get install -y libpmix-dev libpmix-bin

WORKDIR /root
RUN wget https://github.com/openpmix/prrte/releases/download/v3.0.14/prrte-3.0.14.tar.gz && tar xzvf prrte-3.0.14.tar.gz

WORKDIR /root/prrte-3.0.14
RUN ./configure --prefix=/usr/bin

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
