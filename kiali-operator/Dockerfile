FROM quay.io/kiali/kiali-operator:v1.17.0 AS builder

FROM scratch

ENV OPERATOR=/usr/local/bin/ansible-operator \
    USER_NAME=ansible-operator \
    HOME=/opt/ansible \
    USER_UID=1001

COPY --from=builder /opt/ansible/roles/ /opt/ansible/roles/
COPY --from=builder /opt/ansible/playbooks/ /opt/ansible/playbooks/
COPY --from=builder /opt/ansible/watches.yaml /opt/ansible/watches.yaml
COPY --from=builder /usr/local/bin/ansible-operator /usr/local/bin/ansible-operator

LABEL source=git@github.com:kyma-project/kyma.git

ENTRYPOINT [ "/usr/local/bin/ansible-operator", "exec-entrypoint", "ansible", "--watches-file=/opt/ansible/watches.yaml", "$@" ]

USER ${USER_UID}
