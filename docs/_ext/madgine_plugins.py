import json
from pathlib import Path
from docutils import nodes
from sphinx import addnodes
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain, ObjType
from sphinx.roles import XRefRole
from sphinx.util.nodes import make_refnode
from sphinx.util.docutils import SphinxDirective
from docutils.parsers.rst import directives
from docutils.statemachine import StringList
from sphinx.util import logging
logger = logging.getLogger(__name__)

class PluginIndexDirective(SphinxDirective):
    has_content = False
    option_spec = {"maxdepth": directives.nonnegative_int,
                   "caption": directives.unchanged,
                   "hidden": directives.flag}

    def run(self):
        path = Path(self.config.madgine_plugin_data)
        self.env.note_dependency(str(path))       # re-read when a plugin changes        

        plugins = json.loads(path.read_text())["plugins"]

        lines = [".. toctree::", f"   :maxdepth: {self.options.get('maxdepth', 1)}", "   :titlesonly:"]
        if "caption" in self.options:
            lines.append(f"   :caption: {self.options['caption']}")
        if "hidden" in self.options:
            lines.append("   :hidden:")
        lines.append("")

        for plugin in sorted(plugins):
            if plugins[plugin]["tools"]:
                plugins[plugins[plugin]["tools"]]["isTool"] = True

        for plugin in sorted(plugins):
            if "isTool" in plugins[plugin]:
                continue
            lines.append(f"   /{plugins[plugin]['path']}/docs/{plugin.lower()}")

        node = nodes.Element()
        self.state.nested_parse(StringList(lines, source=""), self.content_offset, node)
        return node.children


class PluginDirective(ObjectDescription):
    required_arguments = 1
    has_content = True                      # free-form prose below the generated part

    def handle_signature(self, sig, signode):
        signode += addnodes.desc_name(sig, sig)
        return sig

    def add_target_and_index(self, name, sig, signode):
        node_id = f"plugin-{name}"
        signode["ids"].append(node_id)
        self.env.get_domain("mad").note_plugin(name, node_id, self.env.docname)

    def transform_content(self, content_node):      # runs after the user's prose is parsed
        path = Path(self.config.madgine_plugin_data)
        self.env.note_dependency(str(path))         # rebuild when the JSON changes
        data = json.loads(path.read_text())["plugins"][self.names[0]]

        fields = nodes.field_list()
        def add(label, items):
            if not items: return
            body = nodes.paragraph()
            for i, item in enumerate(items):
                if i: body += nodes.Text(", ")
                body += addnodes.pending_xref("", nodes.literal(item, item),
                    refdomain="mad", reftype="plugin", reftarget=item)
            fields.append(nodes.field("", nodes.field_name("", label), nodes.field_body("", body)))
        add("Depends on", data.get("dependencies", []))
        add("Components", data.get("components", []))
        content_node.insert(0, fields)              # generated part first, prose after


class MadgineDomain(Domain):
    name, label = "mad", "Madgine"
    object_types = {"plugin": ObjType("plugin", "plugin")}
    directives = {"plugin": PluginDirective, "plugin-index": PluginIndexDirective}
    roles = {"plugin": XRefRole()}
    initial_data = {"plugins": {}}

    def note_plugin(self, name, node_id, docname):
        self.data["plugins"][name] = (docname, node_id)

    def clear_doc(self, docname):
        self.data["plugins"] = {k: v for k, v in self.data["plugins"].items() if v[0] != docname}

    def merge_domaindata(self, docnames, otherdata):
        self.data["plugins"].update(otherdata["plugins"])

    def resolve_xref(self, env, fromdocname, builder, typ, target, node, contnode):
        entry = self.data["plugins"].get(target)
        if entry:
            return make_refnode(builder, fromdocname, entry[0], entry[1], contnode, target)


def check_plugin_pages(app, env):                  # event: env-check-consistency
    dom = env.get_domain("mad")
    path = Path(app.config.madgine_plugin_data)    
    data = json.loads(path.read_text())["plugins"]
    known = set(data)
    for name in sorted(known - set(dom.data["plugins"])):
        logger.warning("plugin '%s' has no '.. plugin::' directive in any document", name,
                       type="madgine", subtype="missing")
    for name, (docname, _) in dom.data["plugins"].items():
        if name not in known:
            logger.warning("'.. plugin:: %s' names a plugin without data", name,
                           location=docname, type="madgine", subtype="unknown")


def setup(app):
    app.add_config_value("madgine_plugin_data", "", "env")
    app.add_domain(MadgineDomain)
    app.connect("env-check-consistency", check_plugin_pages)
    return {"parallel_read_safe": True}