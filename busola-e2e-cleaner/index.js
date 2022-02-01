import fetch from "node-fetch";

process.env["NODE_TLS_REJECT_UNAUTHORIZED"] = 0;

const {
  token,
  KUBERNETES_SERVICE_HOST: hostname,
  KUBERNETES_SERVICE_PORT: port,
} = process.env;

const resources = [
  {
    path: "/api/v1/namespaces",
    namePrefix: "a-busola-test-",
  },
  {
    path: "/apis/applicationconnector.kyma-project.io/v1alpha1/applications",
    namePrefix: "test-mock-app-a-busola-test-",
  },
  {
    path: "/apis/storage.k8s.io/v1/storageclasses",
    namePrefix: "a-busola-test-",
  },
  {
    path: "/api/v1/persistentvolumes",
    namePrefix: "test-pv-",
  },
  {
    path: "/apis/rbac.authorization.k8s.io/v1/clusterroles",
    namePrefix: "test-cr-",
  },
  {
    path: "/apis/rbac.authorization.k8s.io/v1/clusterrolebindings",
    namePrefix: "test-crb-",
  },
  {
    path: "/apis/apiextensions.k8s.io/v1/customresourcedefinitions",
    namePrefix: "test-",
  },
];

async function doDelete({ path, namePrefix }) {
  return fetch(`https://${hostname}:${port}${path}`, {
    headers: { Authorization: `Bearer ${token}` },
  })
    .then((r) => r.json())
    .then((data) => {
      const resources = data.items
        .filter((res) => res.metadata.name.startsWith(namePrefix))
        .map(({ metadata }) => ({
          name: metadata.name,
          ageInMinutes:
            (Date.now() - Date.parse(metadata.creationTimestamp)) / 1000 / 60,
        }));

      const resourcesToDelete = resources.filter(
        ({ ageInMinutes }) => ageInMinutes > 60
      );
      if (!resourcesToDelete.length) {
        console.log("nothing to delete");
        return Promise.resolve();
      }

      const promises = resourcesToDelete.map((r) =>
        fetch(`https://${hostname}:${port}${path}/${r.name}`, {
          headers: { Authorization: `Bearer ${token}` },
          method: "DELETE",
        })
      );
      return Promise.allSettled(promises);
    })
    .then((results) => {
      if (results) {
        console.log(results.map((r) => r.value.status));
      }
    })
    .catch(console.log);
}

(async () => {
  for (const resource of resources) {
    console.log("deleting " + resource.path);
    await doDelete(resource);
  }
})();
