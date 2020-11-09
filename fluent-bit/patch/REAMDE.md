# Fluent Bit patches

This folder contains files that will be patched into the current Fluent Bit downloaded version.

## Non blocking write
To solve https://github.com/fluent/fluent-bit/issues/2661 we are patching the PR (https://github.com/fluent/fluent-bit/pull/2672) into the custom image.