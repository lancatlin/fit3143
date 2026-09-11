FROM monashfit/fit3143

USER root
RUN apt-get update && apt-get install -y openssh-server

RUN mkdir -p /var/run/sshd /home/student/.ssh \
  && chmod 700 /home/student/.ssh \
  && chown -R student:student /home/student/.ssh

COPY ./ssh/authorized_keys /home/student/.ssh/authorized_keys

RUN ssh-keygen -A

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
