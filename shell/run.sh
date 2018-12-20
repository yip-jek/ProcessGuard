my_dir=/home/ej/files/workspaces/ProcessGuard

PG=${my_dir}/bin/processguard
DAEMON_FLAG=1
LOG_ID=1001709
CFG_FILE=${my_dir}/etc/pg.cfg

echo "Run: ${PG} ${DAEMON_FLAG} ${LOG_ID} ${CFG_FILE}"
${PG} ${DAEMON_FLAG} ${LOG_ID} ${CFG_FILE}

