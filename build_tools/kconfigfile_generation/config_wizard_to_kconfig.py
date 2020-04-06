#!/usr/bin/python3
# DMOSDK - DecaWave Module Open Software Development Kit
# Copyright (C) 2018-2020 IRIT-RMESS
#
# This file is part of DMOSDK.
#
# DMOSDK is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DMOSDK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DMOSDK.  If not, see <http://www.gnu.org/licenses/>.

import sys
import re


class Tree_item:
    def __init__(self, parent):
        self.parent = parent
        self.childs = []
        self.value = ""
        self.type = ""
        self.options = []

    def __repr__(self):
        return self.value


def entry_bare(stage, entry):
    return '  ' * stage + entry + '\n'


def entry_symbol(stage, entry, value, condition=None):
    base = '  ' * stage + entry + ' ' + value
    if condition:
        return base + ' if ' + condition + '\n'
    return base + '\n'


def entry_prompt(stage, entry, value):
    return '  ' * stage + entry + ' "' + value + '"\n'


def print_tree(kconfig_file, tree, stage=0):
    config = ''
    if tree.type == 'h':
        config += entry_prompt(stage, 'menu', tree.value)
    elif tree.type == 'e':
        define = tree.value.split('-')[0].strip()
        enable = tree.value[tree.value.find('-')+1:].strip()
        desc = 'Configuration'
        config += entry_symbol(stage, 'config', define)
        config += entry_prompt(stage + 1, 'bool', enable)
        if tree.default == '1':
            config += entry_bare(stage + 1, 'default y')
        config += entry_prompt(stage, 'menu', desc)
        config += entry_symbol(stage + 1, 'depends on', define)
    elif tree.type == "q":
        define = tree.value.split('-')[0].strip()
        desc = tree.value[tree.value.find('-')+1:].strip()
        config += entry_symbol(stage, 'config', define)
        config += entry_prompt(stage + 1, 'bool', desc)
        if tree.default == '1':
            config += entry_bare(stage + 1, 'default y')
    elif tree.type == "o":
        define = tree.value.split('-')[0].strip()
        desc = tree.value[tree.value.find('-')+1:].strip()
        if tree.options:
            prefined_value_prefix = 'PREDEFINED_' + define + '_'
            choice = entry_symbol(stage, 'choice', define)
            choice += entry_prompt(stage + 1, 'prompt', desc)
            choice += entry_bare(stage + 1, 'default ' + prefined_value_prefix
                                 + tree.default)
            option_config = entry_symbol(stage, 'config ', define)
            option_config += entry_bare(stage + 1, 'int')
            for option in tree.options:
                choice_def = prefined_value_prefix + option['value']
                choice += entry_symbol(stage + 1, 'config', choice_def)
                choice += entry_prompt(stage + 2, 'bool', option['info'])
                option_config += entry_symbol(
                        stage + 2, 'default', option["value"], choice_def)
            choice += entry_bare(stage, 'endchoice')
            config += choice + option_config
        else:
            config += entry_symbol(stage, 'config', define)
            config += entry_prompt(stage + 1, 'int', tree.value.strip())
            config += entry_bare(stage + 1, 'default ' + tree.default)

    if config:
        kconfig_file.write(config)

    # Use recursivity to print childs
    for child in tree.childs:
        print_tree(kconfig_file, child, stage + 1)

    if tree.type == 'h' or tree.type == 'e':
        kconfig_file.write(entry_bare(stage, 'endmenu'))


def parse_config_wizard(config_wizard_file):

    annotations = [
        'h',
        'e',
        'q',
        'o'
    ]

    autoclosing_annotations = [
        'q',
        'o'
    ]

    config_tree = Tree_item(None)
    current_item = config_tree

    tag_pattern = '<(?P<close>/)?(?P<type>[^<>]*)>\s*(?P<value>[^<]*)'
    tag_re_object = re.compile(tag_pattern)
    define_pattern = '#define\s+[^\s]+\s+(?P<value>.*)'
    define_re_object = re.compile(define_pattern)

    line = config_wizard_file.readline()
    while line:
        line = line.strip()
        match = tag_re_object.search(line)

        if match is None:
            match = define_re_object.match(line)
            if match:
                current_item.default = match.group('value')
                if current_item.type in autoclosing_annotations:
                    current_item = current_item.parent
            line = config_wizard_file.readline()
            continue

        if match.group('type') not in annotations:
            line = config_wizard_file.readline()
            continue

        if match.group('close') is not None:
            current_item = current_item.parent
            line = config_wizard_file.readline()
            continue

        new_item = Tree_item(current_item)
        current_item.childs.append(new_item)
        current_item = new_item
        current_item.value = match.group('value')
        current_item.type = match.group('type')

        if match.group('type') is 'o':
            line = config_wizard_file.readline()
            while '#ifndef' not in line:
                m = re.search('<(\d+?)=>\s*(.+)', line)
                if m:
                    option = {
                        'value': m.group(1),
                        'info': m.group(2).strip()
                    }
                    new_item.options.append(option)
                line = config_wizard_file.readline()

        line = config_wizard_file.readline()
    return config_tree


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Incorrect usage')
        print('Usage: ' + sys.argv[0] + ' CONFIG_WIZARD_PATH KCONFIG_PATH')
        sys.exit(1)

    config_wizard_path = sys.argv[1]
    kconfig_path = sys.argv[2]

    config_wizard_file = None
    try:
        config_wizard_file = open(config_wizard_path, 'r')
    except OSError:
        print('Can\'t open config_wizard_file')
        sys.exit(1)

    kconfig_file = None
    try:
        kconfig_file = open(kconfig_path, 'w')
    except OSError:
        print('Can\'t open kconfig file')
        sys.exit(1)

    config_tree = parse_config_wizard(config_wizard_file)
    print_tree(kconfig_file, config_tree)

    config_wizard_file.close()
    kconfig_file.close()
