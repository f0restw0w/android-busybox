/* vi: set sw=4 ts=4: */
/*
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 *
 * Authors: Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 *
 * Changes:
 * Rani Assaf <rani@magic.metawire.com> 980929: resolve addresses
 * Bernhard Reutner-Fischer rewrote to use index_in_substr_array
 */
//config:config IP
//config:	bool "ip"
//config:	default y
//config:	select PLATFORM_LINUX
//config:	help
//config:	  The "ip" applet is a TCP/IP interface configuration and routing
//config:	  utility. You generally don't need "ip" to use busybox with
//config:	  TCP/IP.
//config:
//config:config FEATURE_IP_ADDRESS
//config:	bool "ip address"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Address manipulation support for the "ip" applet.
//config:
//config:config FEATURE_IP_LINK
//config:	bool "ip link"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Configure network devices with "ip".
//config:
//config:config FEATURE_IP_ROUTE
//config:	bool "ip route"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Add support for routing table management to "ip".
//config:
//config:config FEATURE_IP_ROUTE_DIR
//config:	string "ip route configuration directory"
//config:	default "/etc/iproute2"
//config:	depends on FEATURE_IP_ROUTE
//config:	help
//config:	  Location of the "ip" applet routing configuration.
//config:
//config:config FEATURE_IP_TUNNEL
//config:	bool "ip tunnel"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Add support for tunneling commands to "ip".
//config:
//config:config FEATURE_IP_RULE
//config:	bool "ip rule"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Add support for rule commands to "ip".
//config:
//config:config FEATURE_IP_NEIGH
//config:	bool "ip neighbor"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Add support for neighbor commands to "ip".
//config:
//config:config FEATURE_IP_SHORT_FORMS
//config:	bool "Support short forms of ip commands"
//config:	default y
//config:	depends on IP
//config:	help
//config:	  Also support short-form of ip <OBJECT> commands:
//config:	  ip addr   -> ipaddr
//config:	  ip link   -> iplink
//config:	  ip route  -> iproute
//config:	  ip tunnel -> iptunnel
//config:	  ip rule   -> iprule
//config:	  ip neigh  -> ipneigh
//config:
//config:	  Say N unless you desparately need the short form of the ip
//config:	  object commands.
//config:
//config:config FEATURE_IP_RARE_PROTOCOLS
//config:	bool "Support displaying rarely used link types"
//config:	default n
//config:	depends on IP
//config:	help
//config:	  If you are not going to use links of type "frad", "econet",
//config:	  "bif" etc, you probably don't need to enable this.
//config:	  Ethernet, wireless, infrared, ppp/slip, ip tunnelling
//config:	  link types are supported without this option selected.
//config:
//config:config IPADDR
//config:	bool
//config:	default y
//config:	depends on FEATURE_IP_SHORT_FORMS && FEATURE_IP_ADDRESS
//config:
//config:config IPLINK
//config:	bool
//config:	default y
//config:	depends on FEATURE_IP_SHORT_FORMS && FEATURE_IP_LINK
//config:
//config:config IPROUTE
//config:	bool
//config:	default y
//config:	depends on FEATURE_IP_SHORT_FORMS && FEATURE_IP_ROUTE
//config:
//config:config IPTUNNEL
//config:	bool
//config:	default y
//config:	depends on FEATURE_IP_SHORT_FORMS && FEATURE_IP_TUNNEL
//config:
//config:config IPRULE
//config:	bool
//config:	default y
//config:	depends on FEATURE_IP_SHORT_FORMS && FEATURE_IP_RULE
//config:
//config:config IPNEIGH
//config:	bool
//config:	default y
//config:	depends on FEATURE_IP_SHORT_FORMS && FEATURE_IP_NEIGH

//applet:#if ENABLE_FEATURE_IP_ADDRESS || ENABLE_FEATURE_IP_ROUTE || ENABLE_FEATURE_IP_LINK || ENABLE_FEATURE_IP_TUNNEL || ENABLE_FEATURE_IP_RULE || ENABLE_FEATURE_IP_NEIGH
//applet:IF_IP(APPLET(ip, BB_DIR_SBIN, BB_SUID_DROP))
//applet:#endif
//applet:IF_IPADDR(APPLET(ipaddr, BB_DIR_SBIN, BB_SUID_DROP))
//applet:IF_IPLINK(APPLET(iplink, BB_DIR_SBIN, BB_SUID_DROP))
//applet:IF_IPROUTE(APPLET(iproute, BB_DIR_SBIN, BB_SUID_DROP))
//applet:IF_IPRULE(APPLET(iprule, BB_DIR_SBIN, BB_SUID_DROP))
//applet:IF_IPTUNNEL(APPLET(iptunnel, BB_DIR_SBIN, BB_SUID_DROP))
//applet:IF_IPNEIGH(APPLET(ipneigh, BB_DIR_SBIN, BB_SUID_DROP))

//kbuild:lib-$(CONFIG_IP) += ip.o

/* would need to make the " | " optional depending on more than one selected: */
//usage:#define ip_trivial_usage
//usage:       "[OPTIONS] {"
//usage:	IF_FEATURE_IP_ADDRESS("address | ")
//usage:	IF_FEATURE_IP_ROUTE("route | ")
//usage:	IF_FEATURE_IP_LINK("link | ")
//usage:	IF_FEATURE_IP_TUNNEL("tunnel | ")
//usage:	IF_FEATURE_IP_NEIGH("neigh | ")
//usage:	IF_FEATURE_IP_RULE("rule")
//usage:       "} {COMMAND}"
//usage:#define ip_full_usage "\n\n"
//usage:       "ip [OPTIONS] OBJECT {COMMAND}\n"
//usage:       "where OBJECT := {"
//usage:	IF_FEATURE_IP_ADDRESS("address | ")
//usage:	IF_FEATURE_IP_ROUTE("route | ")
//usage:	IF_FEATURE_IP_LINK("link | ")
//usage:	IF_FEATURE_IP_TUNNEL("tunnel | ")
//usage:	IF_FEATURE_IP_NEIGH("neigh | ")
//usage:	IF_FEATURE_IP_RULE("rule")
//usage:       "}\n"
//usage:       "OPTIONS := { -f[amily] { inet | inet6 | link } | -o[neline] }"
//usage:
//usage:#define ipaddr_trivial_usage
//usage:       "{ {add|del} IFADDR dev STRING | {show|flush}\n"
//usage:       "		[dev STRING] [to PREFIX] }"
//usage:#define ipaddr_full_usage "\n\n"
//usage:       "ipaddr {add|change|replace|delete} IFADDR dev STRING\n"
//usage:       "ipaddr {show|flush} [dev STRING] [scope SCOPE-ID]\n"
//usage:       "	[to PREFIX] [label PATTERN]\n"
//usage:       "	IFADDR := PREFIX | ADDR peer PREFIX\n"
//usage:       "	[broadcast ADDR] [anycast ADDR]\n"
//usage:       "	[label STRING] [scope SCOPE-ID]\n"
//usage:       "	SCOPE-ID := [host | link | global | NUMBER]"
//usage:
//usage:#define iplink_trivial_usage
//usage:       "{ set DEVICE { up | down | arp { on | off } | show [DEVICE] }"
//usage:#define iplink_full_usage "\n\n"
//usage:       "iplink set DEVICE { up | down | arp | multicast { on | off } |\n"
//usage:       "			dynamic { on | off } |\n"
//usage:       "			mtu MTU }\n"
//usage:       "iplink show [DEVICE]"
//usage:
//usage:#define iproute_trivial_usage
//usage:       "{ list | flush | add | del | change | append |\n"
//usage:       "		replace | test } ROUTE"
//usage:#define iproute_full_usage "\n\n"
//usage:       "iproute { list | flush } SELECTOR\n"
//usage:       "iproute get ADDRESS [from ADDRESS iif STRING]\n"
//usage:       "	[oif STRING] [tos TOS]\n"
//usage:       "iproute { add | del | change | append | replace | test } ROUTE\n"
//usage:       "	SELECTOR := [root PREFIX] [match PREFIX] [proto RTPROTO]\n"
//usage:       "	ROUTE := [TYPE] PREFIX [tos TOS] [proto RTPROTO] [metric METRIC]"
//usage:
//usage:#define iprule_trivial_usage
//usage:       "{[list | add | del] RULE}"
//usage:#define iprule_full_usage "\n\n"
//usage:       "iprule [list | add | del] SELECTOR ACTION\n"
//usage:       "	SELECTOR := [from PREFIX] [to PREFIX] [tos TOS] [fwmark FWMARK]\n"
//usage:       "			[dev STRING] [pref NUMBER]\n"
//usage:       "	ACTION := [table TABLE_ID] [nat ADDRESS]\n"
//usage:       "			[prohibit | reject | unreachable]\n"
//usage:       "			[realms [SRCREALM/]DSTREALM]\n"
//usage:       "	TABLE_ID := [local | main | default | NUMBER]"
//usage:
//usage:#define iptunnel_trivial_usage
//usage:       "{ add | change | del | show } [NAME]\n"
//usage:       "	[mode { ipip | gre | sit }]\n"
//usage:       "	[remote ADDR] [local ADDR] [ttl TTL]"
//usage:#define iptunnel_full_usage "\n\n"
//usage:       "iptunnel { add | change | del | show } [NAME]\n"
//usage:       "	[mode { ipip | gre | sit }] [remote ADDR] [local ADDR]\n"
//usage:       "	[[i|o]seq] [[i|o]key KEY] [[i|o]csum]\n"
//usage:       "	[ttl TTL] [tos TOS] [[no]pmtudisc] [dev PHYS_DEV]"
//usage:
//usage:#define ipneigh_trivial_usage
//usage:       "{ show | flush} [ to PREFIX ] [ dev DEV ] [ nud STATE ]"
//usage:#define ipneigh_full_usage "\n\n"
//usage:       "ipneigh { show | flush} [ to PREFIX ] [ dev DEV ] [ nud STATE ]"

#include "libbb.h"

#include "libiproute/utils.h"
#include "libiproute/ip_common.h"

#if ENABLE_FEATURE_IP_ADDRESS \
 || ENABLE_FEATURE_IP_ROUTE \
 || ENABLE_FEATURE_IP_LINK \
 || ENABLE_FEATURE_IP_TUNNEL \
 || ENABLE_FEATURE_IP_RULE \
 || ENABLE_FEATURE_IP_NEIGH

static int FAST_FUNC ip_print_help(char **argv UNUSED_PARAM)
{
	bb_show_usage();
}

typedef int FAST_FUNC (*ip_func_ptr_t)(char**);

static int ip_do(ip_func_ptr_t ip_func, char **argv)
{
	argv = ip_parse_common_args(argv + 1);
	return ip_func(argv);
}

#if ENABLE_FEATURE_IP_ADDRESS
int ipaddr_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int ipaddr_main(int argc UNUSED_PARAM, char **argv)
{
	return ip_do(do_ipaddr, argv);
}
#endif
#if ENABLE_FEATURE_IP_LINK
int iplink_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int iplink_main(int argc UNUSED_PARAM, char **argv)
{
	return ip_do(do_iplink, argv);
}
#endif
#if ENABLE_FEATURE_IP_ROUTE
int iproute_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int iproute_main(int argc UNUSED_PARAM, char **argv)
{
	return ip_do(do_iproute, argv);
}
#endif
#if ENABLE_FEATURE_IP_RULE
int iprule_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int iprule_main(int argc UNUSED_PARAM, char **argv)
{
	return ip_do(do_iprule, argv);
}
#endif
#if ENABLE_FEATURE_IP_TUNNEL
int iptunnel_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int iptunnel_main(int argc UNUSED_PARAM, char **argv)
{
	return ip_do(do_iptunnel, argv);
}
#endif
#if ENABLE_FEATURE_IP_NEIGH
int ipneigh_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int ipneigh_main(int argc UNUSED_PARAM, char **argv)
{
	return ip_do(do_ipneigh, argv);
}
#endif


int ip_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int ip_main(int argc UNUSED_PARAM, char **argv)
{
	static const char keywords[] ALIGN1 =
		IF_FEATURE_IP_ADDRESS("address\0")
		IF_FEATURE_IP_ROUTE("route\0")
		IF_FEATURE_IP_ROUTE("r\0")
		IF_FEATURE_IP_LINK("link\0")
		IF_FEATURE_IP_TUNNEL("tunnel\0")
		IF_FEATURE_IP_TUNNEL("tunl\0")
		IF_FEATURE_IP_RULE("rule\0")
		IF_FEATURE_IP_NEIGH("neigh\0")
		;
	static const ip_func_ptr_t ip_func_ptrs[] = {
		ip_print_help,
		IF_FEATURE_IP_ADDRESS(do_ipaddr,)
		IF_FEATURE_IP_ROUTE(do_iproute,)
		IF_FEATURE_IP_ROUTE(do_iproute,)
		IF_FEATURE_IP_LINK(do_iplink,)
		IF_FEATURE_IP_TUNNEL(do_iptunnel,)
		IF_FEATURE_IP_TUNNEL(do_iptunnel,)
		IF_FEATURE_IP_RULE(do_iprule,)
		IF_FEATURE_IP_NEIGH(do_ipneigh,)
	};
	ip_func_ptr_t ip_func;
	int key;

	argv = ip_parse_common_args(argv + 1);
	key = *argv ? index_in_substrings(keywords, *argv++) : -1;
	ip_func = ip_func_ptrs[key + 1];

	return ip_func(argv);
}

#endif /* any of ENABLE_FEATURE_IP_xxx is 1 */
