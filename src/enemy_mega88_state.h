/* Per-actor transient state: never persisted in pilot saves. */
typedef struct {
 int phase,fired,hit,absorbed,done,stamp,safeX;
 float ax,ay,bx,by,half;
} EnemyMega88;
