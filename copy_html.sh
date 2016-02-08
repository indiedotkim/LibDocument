#!/usr/bin/env bash

set -e

VERSION=`grep project\( CMakeLists.txt | cut -d ' ' -f 3`

for html in docs/html/*.html ; do
    dest=`echo "$html" | sed -E 's/docs\/html\///'`
    sed -E 's/(index)?([^".]*)\.html/\/libdocument\/\2/g' "$html" \
        | sed -E 's/<a href="#/<a class="page-scroll btn btn-primary" href="#/g' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#([^"]+)">More...<\/a>/<a class="page-scroll" href="#\1">More\&hellip;<\/a>/g' \
        | sed -E 's/<h2 class="groupheader"><a name="([^"]+)"/<h2 id="\1" class="groupheader"><a name="\1"/g' \
        | sed -E 's/^([^<].*)<\/h2>/\1<\/h2><dl>/g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/  <\/table>\n  <\/dd>\n<\/dl>/<\/dl><\/div>/g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<\/p>\n<dl class="section return">/<\/p><div><dl class="section return">/' \
        | sed -E 's/<dl class="section return"><dt>Returns<\/dt><dd>(.*)<\/dd><\/dl>/<div class="panel panel-default"><div class="panel-heading">Parameters and Return Value<\/div><dl class="dl-horizontal"><dt>Returns<\/dt><dd>\1<\/dd><\/dl><\/div>/' \
        | sed -E 's/ +(<table class="memname">|<\/table>|<tr>|<\/tr>)//g' \
        | sed -E 's/<td class="memname">(.*)<\/td>/<h4>\1<\/h4>/g' \
        | sed -E 's/<td class="fieldname">(.*)<\/td>/<td class="fieldname"><p><code>\1<\/code><\/p><\/td>/g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<dl class="params"><dt>Parameters<\/dt><dd>\n  <table class="params">/<table class="params"><div class="panel-heading">Parameters and Return Value<\/div><dl class="dl-horizontal">/g' \
        | sed -E 's/<td class="paramtype">(.*[^*])&#160;<\/td>/<td class="paramtype">\1\&nbsp;<\/td>/g' \
        | sed -E 's/<td class="paramname">(.*)<\/td><td>(.*)<\/td><\/tr>/<dt><code>\1<\/code><\/dt><dd>\2<\/dd>/g' \
        | sed -E 's/<table class="([^"]+)">/<div class="panel panel-default \1">/g' \
        | sed -E 's/<tr><th[^>]*>([^<]+)<\/th><\/tr>/<div class="panel-heading">\1<\/div><dl class="dl-horizontal">/g' \
        | sed -E 's/<tr class="memitem:[^>]*><td[^>]*>(.*)<\/td><td[^>]*>(.*)<\/td><\/tr>/<dt class="memitem">\1\2<\/dt>/g' \
        | sed -E 's/<tr class="memitem:[^>]*><td[^>]*>(.*)<\/td><\/tr>/<dt class="memitem">\1<\/dt>/g' \
        | sed -E 's/<tr class="memdesc:[^>]*><td[^>]*>(.*)<\/td><td[^>]*>(.*)<\/td><\/tr>/<dd class="memdesc">\1\2<\/dd>/g' \
        | sed -E 's/<tr class="separator:[^>]*><td[^>]*>(.*)<\/td><\/tr>/<dd class="memdesc">\1<\/dd>/g' \
        | sed -E 's/<tr><td class="([^"]+)"[^>]*>/<dt class="\1">/g' \
        | sed -E 's/<\/td><td class="([^"]+)">/<\/dt><dd class="\1">/g' \
        | sed -E 's/^<\/td><\/tr>/<\/dd>/g' \
        | sed -E 's/<\/table>/<\/dl><\/div>/g' \
        | sed -E 's/class="col-([^ ]+)[^"]*"/class="col-\1"/g' \
        | sed -E 's/class="[^"]+ memberdecls"/class="memberdecls"/g' \
        | sed -E 's/<tr.*><td[^>]*>//g' \
        | sed -E 's/<\/td><\/tr>//g' \
        | sed -E 's/<td[^>]*>/<code>/g' \
        | sed -E 's/<\/td>/<\/code>/g' \
        | sed -E 's/<code>(&#160;| )+/<code>/g' \
        | sed -E 's/(&#160;| )+<\/code>/<\/code>/g' \
        | sed -E 's/<code><\/code>//g' \
        | grep -v -E '^\s*$' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<\/code>\n *<code>//g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<\/h4>\n *<code>//g' \
        | sed -E 's/<h4>(.*)<\/code>/<h4>\1<\/h4>/g' \
        | sed -E 's/<\/code><code>//g' \
        | sed -E 's/,([a-zA-Z<(])/, \1/g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<\/dd>\n<\/dd>/<\/dd>/g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<\/dd>\n<dd class="memdesc">//g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<dl>\n<a /<dl><dt><a /g' \
        | sed -e ':a' -e 'N' -e '$!ba' -e 's/<\/dd>\n<a /<\/dd><dt><a /g' \
        | sed -E 's/}<dd class="memdesc">/}<\/dt><dd class="memdesc">/' \
        | sed -E -e ':a' -e 'N' -e '$!ba' -e 's/ *&#124;\n *<a href="[^"]+">[^<]+<\/a> *//g' \
        | sed -E -e ':a' -e 'N' -e '$!ba' -e 's/<nav (\n|.)*<\/nav>//' \
        | sed -E 's/ &#124;$//' \
        | sed -E -e ':a' -e 'N' -e '$!ba' -e 's/<\/dl><\/div>\n<div class="panel panel-default"><div class="panel-heading">Parameters and Return Value<\/div><dl class="dl-horizontal">//g' \
        | sed -E -e ':a' -e 'N' -e '$!ba' -e 's/<\/dd><\/dl><\/div><\/div>\n<\/div><\/div>/<\/dd><\/dl><\/div><\/div>/' \
        | sed -E 's/<div class="summary">/<div class="summary"><a class="page-scroll btn btn-link" href="\/libdocument"><i class="fa fa-anchor"><\/i><br \/>Back to Overview<\/a><br \/>/' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#nested-classes">Classes/<a class="page-scroll btn btn-primary" href="#nested-classes"><i class="fa fa-cubes"><\/i><br \/>Classes/' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#define-members">Macros/<a class="page-scroll btn btn-primary" href="#define-members"><i class="fa fa-code"><\/i><br \/>Macros/' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#typedef-members">Typedefs/<a class="page-scroll btn btn-primary" href="#typedef-members"><i class="fa fa-dot-circle-o"><\/i><br \/>Typedefs/' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#enum-members">Enumerations/<a class="page-scroll btn btn-primary" href="#enum-members"><i class="fa fa-list"><\/i><br \/>Enumerations/' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#func-members">Functions/<a class="page-scroll btn btn-primary" href="#func-members"><i class="fa fa-share"><\/i><br \/>Functions/' \
        | sed -E 's/<a class="page-scroll btn btn-primary" href="#var-members">Variables/<a class="page-scroll btn btn-primary" href="#var-members"><i class="fa fa-columns"><\/i><br \/>Variables/' \
        | sed -E 's/<div class="container">/<div>/' \
        | sed -E "s/<div class=\"contents\">/<div class=\"contents\"><p class=\"text-right text-muted\">Version: $VERSION<\\/p>/" \
        > ../codamono.com/libdocument/$dest
done

#cp -R docs/html/* ../codamono.com/libdocument

