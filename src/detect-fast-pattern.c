/** Copyright (c) 2009 Open Information Security Foundation.
 *  \author Anoop Saldanha <poonaatsoc@gmail.com>
 */

#include "eidps-common.h"
#include "detect.h"
#include "detect-content.h"
#include "detect-parse.h"
#include "detect-engine.h"
#include "detect-engine-mpm.h"
#include "detect-fast-pattern.h"

#include "util-error.h"
#include "util-debug.h"
#include "util-unittest.h"

int DetectFastPatternSetup(DetectEngineCtx *, Signature *, SigMatch *, char *);
void DetectFastPatternRegisterTests(void);

/**
 * \brief Registration function for fast_pattern keyword
 */
void DetectFastPatternRegister(void) {
    sigmatch_table[DETECT_FAST_PATTERN].name = "fast_pattern";
    sigmatch_table[DETECT_FAST_PATTERN].Match = NULL;
    sigmatch_table[DETECT_FAST_PATTERN].Setup = DetectFastPatternSetup;
    sigmatch_table[DETECT_FAST_PATTERN].Free  = NULL;
    sigmatch_table[DETECT_FAST_PATTERN].RegisterTests = DetectFastPatternRegisterTests;

    sigmatch_table[DETECT_FAST_PATTERN].flags |= SIGMATCH_PAYLOAD;
}

/**
 * \brief Configures the previous content context for a fast_pattern modifier
 *        keyword used in the rule
 *
 * \param de_ctx   Pointer to the Detection Engine Context
 * \param s        Pointer to the Signature to which the current keyword belongs
 * \param m        Pointer to the SigMatch
 * \param null_str Should hold an empty string always
 *
 * \retval  0 On success
 * \retval -1 On failure
 */
int DetectFastPatternSetup(DetectEngineCtx *de_ctx, Signature *s, SigMatch *m,
                           char *null_str)
{
    if (null_str != NULL && strcmp(null_str, "") != 0) {
        SCLogError(SC_INVALID_ARGUMENT, "DetectFastPatternSetup: fast_pattern "
                   "shouldn't be supplied with a value");
        return -1;
    }

    if (m == NULL) {
        SCLogWarning(SC_ERR_INVALID_SIGNATURE, "fast_pattern found inside the "
                     "rule, without any preceding keywords");
        return -1;
    }

    if (m->type != DETECT_CONTENT) {
        SCLogWarning(SC_ERR_INVALID_SIGNATURE, "fast_pattern found inside the "
                     "rule, with content not being its preceding keyword");
        return -1;
    }

    ((DetectContentData *)m->ctx)->flags |= DETECT_CONTENT_FAST_PATTERN;

    return 0;
}

/*----------------------------------Unittests---------------------------------*/

#ifdef UNITTESTS

inline SigGroupHead *SigMatchSignaturesGetSgh(ThreadVars *,
                                              DetectEngineCtx *,
                                              DetectEngineThreadCtx *,
                                              Packet *);

int DetectFastPatternTest01(void)
{
    SigMatch *sm = NULL;
    DetectEngineCtx *de_ctx = NULL;
    int result = 1;

    if ( (de_ctx = DetectEngineCtxInit()) == NULL) {
        result = 0;
        goto end;
    }

    de_ctx->flags |= DE_QUIET;
    de_ctx->sig_list = SigInit(de_ctx, "alert icmp any any -> any any "
                               "(content:\"/one/\"; fast_pattern; "
                               "msg:\"Testing fast_pattern\"; sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    result = 0;
    sm = de_ctx->sig_list->match;
    while (sm != NULL) {
        if (sm->type == DETECT_CONTENT) {
            if ( ((DetectContentData *)sm->ctx)->flags &
                 DETECT_CONTENT_FAST_PATTERN) {
                result |= 1;
                break;
            } else {
                result = 0;
                break;
            }
        }
        sm = sm->next;
    }

 end:
    SigCleanSignatures(de_ctx);
    DetectEngineCtxFree(de_ctx);
    return result;
}

int DetectFastPatternTest02(void)
{
    SigMatch *sm = NULL;
    DetectEngineCtx *de_ctx = NULL;
    int result = 1;

    if ( (de_ctx = DetectEngineCtxInit()) == NULL) {
        result = 0;
        goto end;
    }

    de_ctx->flags |= DE_QUIET;
    de_ctx->sig_list = SigInit(de_ctx, "alert icmp any any -> any any "
                               "(content:\"/one/\"; fast_pattern; "
                               "content:boo; fast_pattern; "
                               "msg:\"Testing fast_pattern\"; sid:1;)");
    if (de_ctx->sig_list == NULL)
        result = 0;

    result = 0;
    sm = de_ctx->sig_list->match;
    while (sm != NULL) {
        if (sm->type == DETECT_CONTENT) {
            if (((DetectContentData *)sm->ctx)->flags &
                DETECT_CONTENT_FAST_PATTERN) {
                result |= 1;
            } else {
                result = 0;
                break;
            }
        }
        sm = sm->next;
    }

 end:
    SigCleanSignatures(de_ctx);
    DetectEngineCtxFree(de_ctx);
    return result;
}

int DetectFastPatternTest03(void)
{
    SigMatch *sm = NULL;
    DetectEngineCtx *de_ctx = NULL;
    int result = 1;

    if ( (de_ctx = DetectEngineCtxInit()) == NULL) {
        result = 0;
        goto end;
    }

    de_ctx->flags |= DE_QUIET;
    de_ctx->sig_list = SigInit(de_ctx, "alert icmp any any -> any any "
                               "(content:\"/one/\"; "
                               "msg:\"Testing fast_pattern\"; sid:1;)");
    if (de_ctx->sig_list == NULL)
        result = 0;

    result = 0;
    sm = de_ctx->sig_list->match;
    while (sm != NULL) {
        if (sm->type == DETECT_CONTENT) {
            if ( !(((DetectContentData *)sm->ctx)->flags &
                   DETECT_CONTENT_FAST_PATTERN)) {
                result |= 1;
            } else {
                result = 0;
                break;
            }
        }
        sm = sm->next;
    }

 end:
    SigCleanSignatures(de_ctx);
    DetectEngineCtxFree(de_ctx);
    return result;
}

int DetectFastPatternTest04(void)
{
    DetectEngineCtx *de_ctx = NULL;
    int result = 1;

    if ( (de_ctx = DetectEngineCtxInit()) == NULL) {
        result = 0;
        goto end;
    }

    de_ctx->flags |= DE_QUIET;
    de_ctx->sig_list = SigInit(de_ctx, "alert icmp any any -> any any "
                               "(content:\"/one/\"; fast_pattern:boo; "
                               "msg:\"Testing fast_pattern\"; sid:1;)");
    if (de_ctx->sig_list == NULL)
        result = 1;

 end:
    SigCleanSignatures(de_ctx);
    DetectEngineCtxFree(de_ctx);
    return result;
}

int DetectFastPatternTest05(void)
{
    uint8_t *buf = (uint8_t *) "Oh strin1.  But what "
        "strin2.  This is strings3.  We strins_str4. we "
        "have strins_string5";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL) {
        goto end;
    }

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings_str4; content:strings_string5; "
                               "sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) != 0)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest06(void)
{
    uint8_t *buf = (uint8_t *) "Oh this is a string1.  But what is this with "
        "string2.  This is strings3.  We have strings_str4.  We also have "
        "strings_string5";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL) {
        goto end;
    }

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings_str4; content:strings_string5; "
                               "sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) != 0)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest07(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  now here comes our "
        "dark knight strings_string5.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL) {
        goto end;
    }

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings_str4; content:strings_string5; "
                               "sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 0)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest08(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  now here comes our "
        "dark knight strings3.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL)
        goto end;

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings_str4; content:strings_string5; "
                               "sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 1)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest09(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  no_strings4 _imp now here "
        "comes our dark knight strings3.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL)
        goto end;

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings4_imp; fast_pattern; "
                               "content:strings_string5; sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 0)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest10(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  strings4_imp now here "
        "comes our dark knight strings5.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL)
        goto end;

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings4_imp; fast_pattern; "
                               "content:strings_string5; sid:1;)");
    if (de_ctx->sig_list == NULL)
        result = 0;


    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 1)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest11(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  strings5_imp now here "
        "comes our dark knight strings5.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL)
        goto end;

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; fast_pattern; "
                               "content:strings4_imp; fast_pattern; "
                               "content:strings_string5; sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 0)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest12(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  strings5_imp now here "
        "comes our dark knight strings5.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL)
        goto end;

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; "
                               "content:strings4_imp; "
                               "content:strings_string5; sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 0)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

int DetectFastPatternTest13(void)
{
    uint8_t *buf = (uint8_t *) "Dummy is our name.  Oh yes.  From right here "
        "right now, all the way to hangover.  right.  strings5_imp now here "
        "comes our dark knight strings_string5.  Yes here is our dark knight";
    uint16_t buflen = strlen((char *)buf);
    Packet p;
    ThreadVars th_v;
    DetectEngineThreadCtx *det_ctx;
    int result = 0;

    memset(&th_v, 0, sizeof(th_v));
    memset(&p, 0, sizeof(p));
    p.src.family = AF_INET;
    p.dst.family = AF_INET;
    p.payload = buf;
    p.payload_len = buflen;
    p.proto = IPPROTO_TCP;

    DetectEngineCtx *de_ctx = DetectEngineCtxInit();
    if (de_ctx == NULL)
        goto end;

    de_ctx->flags |= DE_QUIET;

    de_ctx->sig_list = SigInit(de_ctx, "alert tcp any any -> any any "
                               "(msg:\"fast_pattern test\"; content:string1; "
                               "content:string2; content:strings3; "
                               "content:strings4_imp; "
                               "content:strings_string5; sid:1;)");
    if (de_ctx->sig_list == NULL) {
        result = 0;
        goto end;
    }

    SigGroupBuild(de_ctx);
    PatternMatchPrepare(mpm_ctx, MPM_B2G);
    DetectEngineThreadCtxInit(&th_v, (void *)de_ctx, (void *)&det_ctx);

    /* start the scan phase */
    det_ctx->sgh = SigMatchSignaturesGetSgh(&th_v, de_ctx, det_ctx, &p);
    if (PacketPatternScan(&th_v, det_ctx, &p) == 1)
        result = 1;

    SigGroupCleanup(de_ctx);
    SigCleanSignatures(de_ctx);

    DetectEngineThreadCtxDeinit(&th_v, (void *)det_ctx);
    PatternMatchDestroy(mpm_ctx);
    DetectEngineCtxFree(de_ctx);

end:
    return result;
}

#endif

void DetectFastPatternRegisterTests(void)
{

#ifdef UNITTESTS

    UtRegisterTest("DetectFastPatternTest01", DetectFastPatternTest01, 1);
    UtRegisterTest("DetectFastPatternTest02", DetectFastPatternTest02, 1);
    UtRegisterTest("DetectFastPatternTest03", DetectFastPatternTest03, 1);
    UtRegisterTest("DetectFastPatternTest04", DetectFastPatternTest04, 1);
    UtRegisterTest("DetectFastPatternTest05", DetectFastPatternTest05, 1);
    UtRegisterTest("DetectFastPatternTest06", DetectFastPatternTest06, 1);
    UtRegisterTest("DetectFastPatternTest07", DetectFastPatternTest07, 1);
    UtRegisterTest("DetectFastPatternTest08", DetectFastPatternTest08, 1);
    UtRegisterTest("DetectFastPatternTest09", DetectFastPatternTest09, 1);
    UtRegisterTest("DetectFastPatternTest10", DetectFastPatternTest10, 1);
    UtRegisterTest("DetectFastPatternTest11", DetectFastPatternTest11, 1);
    UtRegisterTest("DetectFastPatternTest12", DetectFastPatternTest12, 1);
    UtRegisterTest("DetectFastPatternTest13", DetectFastPatternTest13, 1);

#endif

}
