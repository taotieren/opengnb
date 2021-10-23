/*
   Copyright (C) gnbdev

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "gnb.h"
#include "gnb_svr.h"
#include "gnb_time.h"
#include "gnb_address.h"
#include "gnb_arg_list.h"
#include "gnb_log.h"

#ifdef _WIN32
#include <windows.h>
#endif

gnb_core_t *gnb_core;

int gnb_daemon();

void save_pid(const char *pid_file);

gnb_conf_t* gnb_argv(int argc,char *argv[]);

void primary_process_loop(gnb_core_t *gnb_core);

extern gnb_arg_list_t *gnb_es_arg_list;

extern int is_self_test;

static void self_test(){

	int i;

	GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest local node=%lu\n", gnb_core->local_node->uuid32);
	GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest tun ipv4[%s]\n",   GNB_ADDR4STR_PLAINTEXT1(&gnb_core->local_node->tun_addr4));

    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest binary_dir='%s'\n", gnb_core->conf->binary_dir);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest conf_dir='%s'\n", gnb_core->conf->conf_dir);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest map_file='%s'\n", gnb_core->conf->map_file);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest pid_file='%s'\n", gnb_core->conf->pid_file);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest log_path='%s'\n", gnb_core->conf->log_path);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest node_cache_file='%s'\n", gnb_core->conf->node_cache_file);

    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest listen='%s'\n", gnb_core->conf->listen_address6_string);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest listen='%s'\n", gnb_core->conf->listen_address4_string);

    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest mtu=%d\n", gnb_core->conf->mtu);

    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest addr_secure=%d\n", gnb_core->conf->addr_secure);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest lite_mode=%d\n", gnb_core->conf->lite_mode);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest public_index_service=%d\n", gnb_core->conf->public_index_service);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest multi_socket=%d\n", gnb_core->conf->multi_socket);
    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest direct_forwarding=%d\n", gnb_core->conf->direct_forwarding);

    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest activate tun=%d\n", gnb_core->conf->activate_tun);

    GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest public index service=%d\n", gnb_core->conf->public_index_service);


    int node_num;
    int j;
    gnb_ctl_block_t *ctl_block;
    gnb_address_list_t *static_address_list;
    gnb_address_t *gnb_address;
    gnb_node_t *node;

    if(1 == gnb_core->conf->activate_tun && 0 == gnb_core->conf->public_index_service){

    	ctl_block = gnb_core->ctl_block;

        node_num = ctl_block->node_zone->node_num;

        for(i=0; i<node_num; i++){

            node = &ctl_block->node_zone->node[i];

            if (node->uuid32 != ctl_block->core_zone->local_uuid){
                continue;
            }

            static_address_list  = (gnb_address_list_t *)&node->static_address_block;

            for( j=0; j<static_address_list->num; j++ ){

                gnb_address = &static_address_list->array[j];

                if (0==gnb_address->port){
                    continue;
                }

                GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest address %s\n", GNB_IP_PORT_STR1(gnb_address));

            }

        }

        GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest num of index=%d\n", gnb_core->index_address_ring.address_list->num);

        for(i=0; i< gnb_core->index_address_ring.address_list->num; i++){
            GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest index node '%s'\n", GNB_IP_PORT_STR1(&gnb_core->index_address_ring.address_list->array[i]));
        }

        GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest num of fwd node=%d\n", gnb_core->fwd_node_ring.num);

        for(i=0; i<gnb_core->fwd_node_ring.num; i++){
        	GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest fwd node=%d\n", gnb_core->fwd_node_ring.nodes[i]->uuid32);
        }

        for(i=0; i<gnb_es_arg_list->argc; i++){
            GNB_LOG1(gnb_core->log,GNB_LOG_ID_CORE,"selftest gnb_es argv[%d]='%s'\n", i, gnb_es_arg_list->argv[i]);
        }

    }

}


#ifdef _WIN32

BOOL CALLBACK CosonleHandler(DWORD ev) {

    switch (ev) {

    case CTRL_CLOSE_EVENT:
    case    CTRL_C_EVENT:
        gnb_core_stop(gnb_core);
        exit(0);
    default:
        exit(0);
        break;
    }

    return TRUE;
}

#endif



int main (int argc,char *argv[]){

    gnb_conf_t *conf;

    int pid;

    int ret;

    #ifdef _WIN32
    WSADATA wsaData;
    int err;
    err = WSAStartup(MAKEWORD(2, 2), &wsaData );
    SetConsoleCtrlHandler(CosonleHandler, TRUE);
    #endif

    conf = gnb_argv(argc, argv);

    if ( 0 == conf->public_index_service ) {

        gnb_core = gnb_core_create(conf);

    } else {

        gnb_core = gnb_core_index_service_create(conf);

    }

    if (NULL==gnb_core){
        printf("gnb core create error\n");
        return 1;
    }

    free(conf);

    #ifdef __UNIX_LIKE_OS__
    if (gnb_core->conf->daemon){
        gnb_daemon();
    }

    save_pid(gnb_core->conf->pid_file);
    #endif

    ret = gettimeofday(&gnb_core->now_timeval,NULL);

    if(1==is_self_test){
    	self_test();
    }

    if ( 0 == conf->public_index_service ) {
        gnb_core_start(gnb_core);
    } else {
        gnb_core_index_service_start(gnb_core);
    }

    primary_process_loop(gnb_core);

    #ifdef _WIN32
    WSACleanup();
    #endif

    return 0;

}

