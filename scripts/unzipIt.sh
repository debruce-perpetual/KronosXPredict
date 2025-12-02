#!/bin/bash

set -e

script_dir=$(dirname "${BASH_SOURCE[0]}")
proj_dir=$(realpath ${script_dir}/..)
proj_name=$(basename ${proj_dir})
echo proj_dir=${proj_dir}
echo proj_name=${proj_name}

cd ${proj_dir}

# Find the latest matching zip file in ~/Downloads
zip_file=$(ls -t ~/Downloads/"$proj_name"*.zip 2>/dev/null | head -n 1)
echo "zip_file=${zip_file}"

if [[ -z "$zip_file" ]]; then
    echo "No matching zip file found in ~/Downloads for: $proj_name"
    exit 1
fi

echo "Validating structure of: $zip_file"
echo

# Extract file list and check structure
INVALID=0
while IFS= read -r line; do
    file_path=$(echo "$line" | awk '{for (i=4; i<NF; ++i) printf $i " "; print $NF}')
    file_path=$(echo "$file_path" | sed 's/^\s*//;s/\s*$//')
    echo $file_path

    # Skip summary/footer lines
    [[ -z "$file_path" ]] && continue
    [[ "$file_path" == "Name" ]] && continue

    # Check for relative traversal
    if [[ "$file_path" == *".."* ]]; then
        echo "❌ Path contains '..': $file_path"
        invalid=1
    elif [[ "$file_path" != "$proj_name/"* ]]; then
        echo "❌ Path does not start with '$proj_name/': $file_path"
        invalid=1
    fi
done < <(unzip -l "$zip_file" | tail -n +4 | head -n -2)
echo

if [[ $invalid -eq 1 ]]; then
    echo "❌ Aborting unzip due to invalid paths."
    exit 1
fi

echo "✅ Structure validated. Unzipping into ${proj_dir}/.."
unzip -o "$zip_file" -d "${proj_dir}/.."
find . -type f -not -path './.git/*' -exec touch -m {} +
find . -type d -not -path './.git/*' -exec touch -m {} +

# Ensure all bash or python scripts in scripts/ are executable
scripts_dir="$${proj_dir}/scripts"
if [ -d "$scripts_dir" ]; then
    for file in "$scripts_dir"/*; do
        if [ -f "$file" ]; then
            head_line=$(head -n 1 "$file")
            if [[ "$head_line" =~ ^\#\!.*(bash|python) ]]; then
                echo "🔧 Setting executable on $file ($(echo "$head_line" | awk '{print $2}'))"
                chmod +x "$file"
            fi
        fi
    done
fi
