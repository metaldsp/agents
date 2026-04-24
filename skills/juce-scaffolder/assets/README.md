# <PROJECT_NAME>

<PROJECT_DESCRIPTION>

## Build

If any submodule is hosted in a private GitHub repository, HTTPS access will be denied. Rewrite the URL to use SSH before initialising submodules:

```sh
git config --global url."git@github.com:".insteadOf "https://github.com/"
```

Build:

```sh
cmake -S . -B build
cmake --build build
```
