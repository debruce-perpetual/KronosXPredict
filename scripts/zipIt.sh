#!/bin/bash

set -e

script_dir=$(dirname "${BASH_SOURCE[0]}")
proj_dir=$(realpath ${script_dir}/..)
proj_name=$(basename ${proj_dir})
zip_name="${proj_name}.zip"
echo proj_dir=${proj_dir}
echo proj_name=${proj_name}
echo zip_name=${zip_name}

echo "📦 Creating zip archive... ~/${zip_name}"
cd ${proj_dir}/..
rm -f ~/"${zip_name}"
zip -r ~/"${zip_name}" "${proj_name}" -x '*/.git/*' '*/.gitignore' '*/build/*' '*/dist/*' '*/__pycache__/*' '*/.pytest_cache/*'
