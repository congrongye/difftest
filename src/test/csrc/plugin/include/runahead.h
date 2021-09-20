/***************************************************************************************
* Copyright (c) 2020-2021 Institute of Computing Technology, Chinese Academy of Sciences
* Copyright (c) 2020-2021 Peng Cheng Laboratory
*
* XiangShan is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef RUNAHEAD_H
#define RUNAHEAD_H

#include <queue>
#include "common.h"
#include "difftest.h"
#include "ram.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct RunaheadCheckpoint {
  pid_t pid;
  uint64_t checkpoint_id;
  uint64_t pc;
} RunaheadCheckpoint;

typedef struct RunaheadRequest {
  long int message_type;
  uint64_t target_pc;
} RunaheadRequest;

typedef struct RunaheadResponsePid {
  long int message_type;
  pid_t pid;
} RunaheadResponsePid;

typedef struct RunaheadResponseQuery {
  long int message_type;
  // TODO
} RunaheadResponseQuery;

class Runahead: public Difftest {
public:
  Runahead(int coreid);
  pid_t free_checkpoint();
  void recover_checkpoint(uint64_t checkpoint_id);
  void restart();
  void update_debug_info(void* dest_buffer);
  void run_first_instr();
  int step();
  bool checkpoint_num_exceed_limit();
  int do_instr_runahead();
  pid_t do_instr_runahead_pc_guided(uint64_t jump_target_pc);
  pid_t init_runahead_slave();
  pid_t fork_runahead_slave();
  void runahead_slave();
  pid_t request_slave_runahead();
  pid_t request_slave_runahead_pc_guided(uint64_t target_pc);
  void debug_print_checkpoint_list();

  void do_first_instr_runahead();

  difftest_core_state_t *dut_ptr;
  difftest_core_state_t *ref_ptr;
  // Note: dut & ref does not contain valid value in runahead
  // use dut_ptr & ref_ptr instead.
  // To be refactored later. 

private:
#define RUN_AHEAD_CHECKPOINT_SIZE 64
  std::deque<RunaheadCheckpoint> checkpoints;
};

extern Runahead** runahead;
int init_runahead_slave();
int runahead_init();
int runahead_step();

enum {
  RUNAHEAD_MSG_REQ_ALL,
  RUNAHEAD_MSG_REQ_RUN,
  RUNAHEAD_MSG_REQ_GUIDED_EXEC,
  RUNAHEAD_MSG_REQ_QUERY
};

enum {
  RUNAHEAD_MSG_RESP_ALL,
  RUNAHEAD_MSG_RESP_GUIDED_EXEC,
  RUNAHEAD_MSG_RESP_QUERY,
};

#endif