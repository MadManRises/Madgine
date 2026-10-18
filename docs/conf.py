# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import sys
from pathlib import Path

sys.path.append(str(Path('_ext').resolve()))

project = 'Madgine'
copyright = '2025, Martin Schütz'
author = 'Martin Schütz'
version = os.getenv("MADGINE_VERSION")

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
	'sphinx_rtd_theme',
	'sphinx.ext.graphviz',
	'madgine_plugins'
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'util']
master_doc = 'docs/README'


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    "collapse_navigation" : False
}
html_static_path = ['_static']

primary_domain = 'mad'
madgine_plugin_data = 'docs/plugins.json'
