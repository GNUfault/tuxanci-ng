#ifndef MSG_H
#define MSG_H

#ifdef DEBUG
#define DEBUG_MSG(msg,arg...)	printf(msg, ##arg)
#else /* DEBUG */
#define DEBUG_MSG(msg,arg...) 
#endif /* DEBUG * */

#ifdef DEBUG
#define debug(msg,arg...)	fprintf(stdout, "%s ", _("[Debug]"));   fprintf(stdout, _(msg), ##arg); fprintf(stdout, "\n")
#else /* DEBUG */
#define debug(msg,arg...)
#endif /* DEBUG */

#define warning(msg,arg...)	fprintf(stderr, "%s ", _("[Warning]")); fprintf(stderr, _(msg) "\n", ##arg)
#define error(msg,arg...)	fprintf(stderr, "%s ", _("[Error]"));   fprintf(stderr, _(msg) "\n", ##arg)
#define fatal(msg,arg...)								\
{											\
	fprintf(stderr, "%s ", _("[Fatal]")); fprintf(stderr, _(msg) "\n", ##arg);	\
	abort(0);									\
}

#endif /* MSG_H */
