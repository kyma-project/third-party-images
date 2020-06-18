FROM quay.io/kiali/kiali-operator:v1.19.0 AS builder

FROM alpine:3.11

ENV OPERATOR=/usr/local/bin/ansible-operator \
    USER_UID=1001 \
    USER_NAME=ansible-operator\
    HOME=/opt/ansible

 RUN mkdir -p /etc/ansible && mkdir -p /tmp/ansible-operator \
    && echo "localhost ansible_connection=local" > /etc/ansible/hosts \
    && echo '[defaults]' > /etc/ansible/ansible.cfg \
    && echo 'roles_path = /opt/ansible/roles' >> /etc/ansible/ansible.cfg \
    && echo 'library = /usr/share/ansible/openshift' >> /etc/ansible/ansible.cfg

RUN apk add --no-cache bash build-base curl tini libffi-dev openssl-dev python3-dev py-pip py3-setuptools \
 && pip3 install --no-cache-dir --ignore-installed ipaddress \
    ansible-runner==1.3.4 \
    ansible-runner-http==1.0.0 \
    openshift~=0.10.0 \
    ansible~=2.9 \
    jmespath \
 && apk del build-base libffi-dev openssl-dev python3-dev

COPY --from=builder /opt/ansible/roles/ /opt/ansible/roles/
COPY --from=builder /opt/ansible/playbooks/ /opt/ansible/playbooks/
COPY --from=builder /opt/ansible/watches.yaml /opt/ansible/watches.yaml
COPY --from=builder /usr/local/bin/ansible-operator /usr/local/bin/ansible-operator

RUN mv /usr/bin/ansible-runner /usr/local/bin/ansible-runner

ADD entrypoint /usr/local/bin/entrypoint
ADD user_setup /usr/local/bin/user_setup
RUN /usr/local/bin/user_setup

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["/sbin/tini", "--", "/usr/local/bin/entrypoint"]

USER ${USER_UID}
