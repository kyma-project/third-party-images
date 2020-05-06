FROM quay.io/kiali/kiali-operator:v1.17.0 AS builder

FROM registry.access.redhat.com/ubi8/ubi

COPY --from=builder /opt/ansible/roles/ /opt/ansible/roles/
COPY --from=builder /opt/ansible/playbooks/ /opt/ansible/playbooks/
COPY --from=builder /opt/ansible/watches.yaml /opt/ansible/watches.yaml
COPY --from=builder /usr/local/bin/ /usr/local/bin/

RUN mkdir -p /etc/ansible \
    && echo "localhost ansible_connection=local" > /etc/ansible/hosts \
    && echo '[defaults]' > /etc/ansible/ansible.cfg \
    && echo 'roles_path = /opt/ansible/roles' >> /etc/ansible/ansible.cfg \
    && echo 'library = /usr/share/ansible/openshift' >> /etc/ansible/ansible.cfg

ENV OPERATOR=/usr/local/bin/ansible-operator \
    USER_UID=1001 \
    USER_NAME=ansible-operator\
    HOME=/opt/ansible

# Install python dependencies
# Ensure fresh metadata rather than cached metadata in the base by running
# yum clean all && rm -rf /var/yum/cache/* first
RUN yum clean all && rm -rf /var/cache/yum/* \
 && yum -y update \
 && yum install -y libffi-devel openssl-devel python36-devel gcc python3-pip python3-setuptools \
 && pip3 install --no-cache-dir --ignore-installed ipaddress \
      ansible-runner==1.3.4 \
      ansible-runner-http==1.0.0 \
      openshift~=0.10.0 \
      ansible~=2.9 \
      jmespath \
 && yum remove -y gcc libffi-devel openssl-devel python36-devel \
 && yum clean all \
 && rm -rf /var/cache/yum

ADD user_setup /usr/local/bin/user_setup
RUN /usr/local/bin/user_setup

# Ensure directory permissions are properly set
RUN mkdir -p ${HOME}/.ansible/tmp \
 && chown -R ${USER_UID}:0 ${HOME} \
 && chmod -R ug+rwx ${HOME}

RUN TINIARCH=$(case $(arch) in x86_64) echo -n amd64 ;; ppc64le) echo -n ppc64el ;; *) echo -n $(arch) ;; esac) \
  && curl -L -o /tini https://github.com/krallin/tini/releases/latest/download/tini-$TINIARCH \
  && chmod +x /tini

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT ["/tini", "--", "/usr/local/bin/entrypoint"]

#USER ${USER_UID}
