#!/usr/bin/env bash
# Synchronize the public OSHWHub project and board metadata used for exports.
#
# OSHWHub exposes project metadata anonymously. The Pro editor builds .epro2
# and .net exports client-side from protected source/history APIs; those files
# cannot be obtained from the public metadata endpoints used here.

set -Eeuo pipefail

BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

for tool in curl jq; do
    command -v "$tool" >/dev/null || { echo "ERROR: missing required command: $tool" >&2; exit 2; }
done

request_json() {
    local url="$1" out="$2" referer="${3:-https://oshwhub.com/}"
    local tmp="${out}.tmp"
    curl -fsSL --retry 2 --connect-timeout 15 --max-time 180 -o "$tmp" \
        -H "Cookie: ${OSHWHUB_COOKIE:-}" \
        -H 'User-Agent: Mozilla/5.0' \
        -H 'Accept: application/json' \
        -H "Referer: $referer" \
        "$url"
    jq -e . "$tmp" >/dev/null
    mv "$tmp" "$out"
}

sync_project_metadata() {
    local folder="$1" metadata="$folder/metadata.json" uuid url out images
    uuid="$(jq -r '.uuid // .api_detail.uuid // empty' "$metadata")"
    url="$(jq -r '.url // (.api_detail.path | if . then "https://oshwhub.com/" + . else empty end) // empty' "$metadata")"
    [[ -n "$uuid" ]] || { echo "ERROR: no project UUID in $metadata" >&2; return 1; }
    out="$folder/project_download.json"
    request_json "https://oshwhub.com/api/project/$uuid" "$out" "${url:-https://oshwhub.com/}"
    jq -e --arg uuid "$uuid" '.success == true and .result.uuid == $uuid' "$out" >/dev/null \
        || { echo "ERROR: invalid OSHWHub metadata response for $uuid" >&2; return 1; }
    images="$folder/pro_images.json"
    request_json "https://oshwhub.com/api/project/$uuid/proImages" "$images" "${url:-https://oshwhub.com/}"
    jq -e '.success == true and (.result.boards | arrays)' "$images" >/dev/null \
        || { echo "ERROR: invalid board metadata response for $uuid" >&2; return 1; }
    printf 'metadata: %s (%s PCB documents)\n' \
        "$(basename "$folder")" \
        "$(jq '[.result.boards[]?.pcb, .result.out_pcbs[]?.uuid] | map(select(. != null)) | unique | length' "$images")"
}

shopt -s nullglob
folders=("$BASE_DIR"/*)
for folder in "${folders[@]}"; do
    [[ -f "$folder/metadata.json" ]] || continue
    sync_project_metadata "$folder"
done

echo
echo "Public metadata synchronization complete."
echo "Export .epro2 and .net in an authenticated Pro editor session; the source/history APIs return 401 anonymously."
echo "Run verify_typical_circuits.sh to inspect .net/.epro2 coverage."
