#!/bin/sh
set -e

flatpak-builder --force-clean --repo=repo build manifest.json
flatpak build-bundle repo io.github.GNUfault.tuxanci-x86_64.flatpak io.github.GNUfault.tuxanci