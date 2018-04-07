#!/bin/bash

# url configuration
URL="https://forgge.github.io/theCore/"

# values: always hourly daily weekly monthly yearly never
FREQ="daily"

# begin new sitemap
exec 1> sitemap.xml

# print head
echo '<?xml version="1.0" encoding="UTF-8"?>'
echo '<!-- generator="Milkys Sitemap Generator, https://github.com/mcmilk/sitemap-generator" -->'
echo '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">'

# print urls
IFS=$'\r\n' GLOBIGNORE='*' command eval "OPTIONS=($(cat $0.options))"
find . -type f "${OPTIONS[@]}" -printf "%TY-%Tm-%Td%p\n" | \
while read -r line; do
  DATE=${line:0:10}
  FILE=${line:12}
  echo "<url>"
  echo " <loc>${URL}${FILE}</loc>"
  echo " <lastmod>$DATE</lastmod>"
  echo " <changefreq>$FREQ</changefreq>"
  echo "</url>"
done

# print foot
echo "</urlset>"
