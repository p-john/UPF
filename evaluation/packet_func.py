import os, time, subprocess

class Action:
    action = ""
    target = ""

    def __init__(self, action, target):
        self.action = action
        self.target = target

    def __str__(self):
        return self.action + "  " + self.target


class Range:
    lower = 0
    upper = 0

    def __init__(self, lower, upper):
        self.lower = lower
        self.upper = upper

    def __eq__(self,other):
        return self.lower == other.lower and self.upper == other.upper

    def value_in_range(self, value):
        if value >= self.lower and value <= self.upper:
            return True
        else:
            return False

    def __str__(self):
        return "Range:" + str(self.lower) + " - " + str(self.upper)


class Rule:
    l3src = Range(0, 0)
    l3dst = Range(0, 0)
    l4protocol = Range(0, 0)
    l4src = Range(0, 0)
    l4dst = Range(0, 0)

    def __eq__(self,other):
        if isinstance(other,Rule):
            return  self.l3src == other.l3src and \
                    self.l3dst == other.l3dst and \
                    self.l4protocol == other.l4protocol and \
                    self.l4src == other.l4src and \
                    self.l4dst == other.l4dst
        return NotImplemented

    def set_field(self, range, field_number):
        if field_number == 5:
            self.l3src = range
        elif field_number == 6:
            self.l3dst = range
        elif field_number == 7:
            self.l4protocol = range
        elif field_number == 8:
            self.l4src = range
        elif field_number == 9:
            self.l4dst = range

    def set_action(self, action):
        self.action = action

    def __str__(self):
        return  str(self.l3src) + "\n" + \
                str(self.l3dst) + "\n" + \
                str(self.l4protocol) + "\n" + \
                str(self.l4src) + "\n" + \
                str(self.l4dst) + "\n" + \
                str(self.action)


class Ruleset:
    name = ""
    rules = []

    def __init__(self, name):
        self.name = name
        self.rules = []

    def add_rule(self, rule):
        self.rules.append(rule)

    def __len__(self):
        return len(self.rules)

    def __str__(self):
        keks = ""
        for rule in self.rules:
            keks += str(rule) + "\n"
        return keks


class Packet:
    l3src = 0
    l3dst = 0
    l4protocol = 0
    l4src = 0
    l4dst = 0

    def __init__(self, l3src, l3dst, l4src, l4dst, l4protocol):
        self.l3src = l3src
        self.l3dst = l3dst
        self.l4src = l4src
        self.l4dst = l4dst
        self.l4protocol = l4protocol

    def __str__(self):
        msg = "Sent Packet: " + \
              self.l3src + " " + \
              self.l3dst + " " + \
              self.l4src + " " + \
              self.l4dst + " " + \
              self.l4protocol + " "
        return msg

    def __repr__(self):
        msg = "Sent Packet: " + \
              self.l3src + " " + \
              self.l3dst + " " + \
              self.l4src + " " + \
              self.l4dst + " " + \
              self.l4protocol + " "
        return msg

    def matched(self, rule):
        if rule.l3src.value_in_range(int(self.l3src)) and \
                rule.l3dst.value_in_range(int(self.l3dst)) and \
                rule.l4protocol.value_in_range(int(self.l4protocol)) and \
                rule.l4src.value_in_range(int(self.l4src)) and \
                rule.l4dst.value_in_range(int(self.l4dst)):
            return True
        else:
            return False

def create_packets_from_trace_file(trace_file):
    packets = []
    for line in open(trace_file, 'r'):
        values = line.split()
        l3src = values[0]
        l3dst = values[1]
        l4src = values[2]
        l4dst = values[3]
        protocol = values[4]
        packets.append(Packet(l3src, l3dst, l4src, l4dst, protocol))
    return packets
