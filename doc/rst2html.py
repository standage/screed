#!/usr/bin/env python2.5

try:
    import locale
    locale.setlocale(locale.LC_ALL, '')
except:
    pass

from docutils.core import publish_cmdline, default_description

description = ('Generates (X)HTML documents from standalone '
                'reStructuredText sources. ' + default_description)

publish_cmdline(writer_name='html', description=description)
