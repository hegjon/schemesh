/**
 * Copyright (C) 2023 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "eval.h"

void define_library_io(void) {

#define SCHEMESH_LIBRARY_IO_EXPORT "make-chargbuffer-input-port "

  eval("(library (schemesh io (0 1))\n"
       "  (export " SCHEMESH_LIBRARY_IO_EXPORT ")\n"
       "  (import\n"
       "    (chezscheme)\n"
       "    (schemesh containers))\n"
       "\n"
       /* helper for input port wrapping a chargbuffer */
       "(define-record-type inp\n"
       "  (fields\n"
       "    (immutable source)\n"
       "    (mutable   pos)))\n"
       "\n"
       "(define (inp-block-read p str n)\n"
       "  (assert (fx<=? n (length str)))\n"
       "  (let* ((source     (inp-source p))\n"
       "         (source-pos (inp-pos p))\n"
       "         (source-len (chargbuffer-length source))\n"
       "         (ret-n  (fxmin n (fx- source-len source-pos))))\n"
       "    (cond\n"
       "      ((fx>=? source-pos source-len) (eof-object))\n"
       "      ((fx<=? ret-n      0)          0)\n"
       "      (#t\n"
       "        (do ((i 0 (fx1+ i)))\n"
       "             ((fx>=? i ret-n)\n"
       "               (inp-pos-set! p (fx+ ret-n source-pos))\n"
       "               ret-n)\n"
       "          (string-set! str i (chargbuffer-ref source (fx+ i source-pos))))))))\n"
       "\n"
       "(define (inp-file-position p . pos-or-null)\n"
       "  (if (null? pos-or-null)\n"
       "    (inp-pos p)\n"
       "    (let ((pos (cdr pos-or-null)))\n"
       "      (if (and (fixnum? pos)\n"
       "               (fx>=? pos 0)\n"
       "               (fx<=? pos (chargbuffer-length (inp-source p))))\n"
       "        (inp-pos-set! p pos)\n"
       "        (raise (make-i/o-invalid-position-error pos))))))\n"
       "\n"
       "(define (inp-file-length p)\n"
       "  (chargbuffer-length (inp-source p)))\n"
       "\n"
       "(define (inp-peek-char p)\n"
       "  (let* ((source     (inp-source p))\n"
       "         (source-pos (inp-pos p))\n"
       "         (source-len (chargbuffer-length source)))\n"
       "    (if (fx<? source-pos source-len)\n"
       "      (chargbuffer-ref source source-pos)\n"
       "      (eof-object))))\n"
       "\n"
       "(define (inp-read-char p)\n"
       "  (let* ((source     (inp-source p))\n"
       "         (source-pos (inp-pos p))\n"
       "         (source-len (chargbuffer-length source)))\n"
       "    (if (fx<? source-pos source-len)\n"
       "      (begin\n"
       "        (inp-pos-set! p (fx1+ source-pos))\n"
       "        (chargbuffer-ref source source-pos))\n"
       "      (eof-object))))\n"
       "\n"
       "(define (inp-unread-char ch p)\n"
       "  (let ((source     (inp-source p))\n"
       "        (source-pos (inp-pos p)))\n"
       "    (if (fx>? source-pos 0)\n"
       "      (let ((pos (fx1- source-pos)))\n"
       "        (assert (char=? ch (chargbuffer-ref source pos)))\n"
       "        (inp-pos-set! p pos))\n"
       "      (raise (make-i/o-invalid-position-error 0)))))\n"
       "\n"
       /* create an input port wrapping a chargbuffer */
       "(define (make-chargbuffer-input-port cgb)\n"
       "  (let ((p (make-inp cgb 0)))\n"
       "    (make-input-port\n"
       "      (lambda args\n"
       /*       (format #t \"~s~%\" args) */
       "        (record-case args\n"
       "          (block-read (port str n)    (inp-block-read p str n))\n"
       "          (char-ready? (port)         #t)\n"
       "          (clear-input-port (port)    (void))\n"
       "          (close-port  (port)         (mark-port-closed! p))\n"
       "          (file-position (port . pos) (apply inp-file-position p pos))\n"
       "          (file-length (port)         (inp-file-length p))\n"
       "          (peek-char (port)           (inp-peek-char p))\n"
       "          (port-name (port)           \"chargbuffer-input-port\")\n"
       "          (read-char (port)           (inp-read-char p))\n"
       "          (unread-char (ch port)      (inp-unread-char ch p))\n"
       "          (else (assertion-violationf 'chargbuffer-input-port\n"
       "                    \"operation ~s not handled\" (if (pair? args) (car args) '())))))\n"
       "      \"\")))\n"
       "\n"
       ")\n"); /* close library */

  eval("(import (schemesh io))\n");
}
