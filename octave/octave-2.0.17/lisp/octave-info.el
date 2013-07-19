;;; octave-info.el --- octave-info package for Emacs.

;; Modified for emx by Eberhard Mattes, Dec 1995
;; Modified for Octave by Klaus Gebhardt, Apr 1996 - Mar 1997

;; Copyright (C) 1985, 1986, 1992, 1993, 1994 Free Software Foundation, Inc.

;; Maintainer: FSF
;; Keywords: help

;; This file is part of GNU Emacs.

;; GNU Emacs is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs; see the file COPYING.  If not, write to
;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;; Commentary:

;;; Note that nowadays we expect info files to be made using makeinfo.

;;; Code:

(require 'info)

(defvar Octave-Info-index-strings nil
  "List of the index strings.")

(defvar Octave-Info-history nil
  "List of octave-info nodes user has visited.
Each element of list is a list (FILENAME NODENAME BUFFERPOS).")

(defvar Octave-Info-enable-active-nodes t
  "Non-nil allows Octave-Info to execute Lisp code associated with nodes.
The Lisp code is executed when the node is selected.")

(defvar Octave-Info-fontify t
  "*Non-nil enables highlighting and fonts in Octave-Info nodes.")

(defvar Octave-Info-fontify-maximum-menu-size 30000
  "*Maximum size of menu to fontify if `Octave-Info-fontify' is non-nil.")

(defvar Octave-Info-current-file nil
  "Octave-Info file that Octave-Info is now looking at, or nil.")

(defvar Octave-Info-current-subfile nil
  "Octave-Info subfile that is actually in the *octave-info* buffer now,
or nil if current octave-info file is not split into subfiles.")

(defvar Octave-Info-current-node nil
  "Name of node that Octave-Info is now looking at, or nil.")

(defvar Octave-Info-tag-table-marker (make-marker)
  "Marker pointing at beginning of current Octave-Info file's tag table.
Marker points nowhere if file has no tag table.")

(defvar Octave-Info-current-file-completions nil
  "Cached completion list for current Octave-Info file.")

(defvar Octave-Info-index-alternatives nil
  "List of possible matches for last Octave-Info-index command.")

(defvar Octave-Info-running nil
  "*Non-nil means Octave-Info has already been started.")

;;;###autoload (add-hook 'same-window-buffer-names "*octave-info*")

;;;###autoload
(defun octave-info (index-strings)
  "Enter Octave-Info, the documentation browser."
  (if (or (not Octave-Info-running) t)
      (progn
	(setq Octave-Info-running t)
	(Octave-Info-goto-node (format "(%s)" (car index-strings)))
	(setq Octave-Info-index-strings (cdr index-strings))
	(if (and (not (equal (car Octave-Info-index-strings) "(null)"))
		 (not (equal (car Octave-Info-index-strings) "")))
	    (Octave-Info-index (car Octave-Info-index-strings)))
	(setq Octave-Info-index-strings (cdr Octave-Info-index-strings)))
    (progn
      (setq Octave-Info-index-strings (append Octave-Info-index-strings 
					      index-strings)))))

(defun Octave-Info-error (fmt &optional string)
  (setq Octave-Info-index-strings (cdr Octave-Info-index-strings))
  (Octave-Info-exit)
  (error fmt string))

(defun Octave-Info-find-node (filename nodename &optional no-going-back)
  (if filename
      (let (temp temp-downcase found)
	(setq filename (substitute-in-file-name filename))
	(if (string= (downcase (file-name-nondirectory filename)) "dir")
	    (setq found t)
	  (let ((dirs (if (string-match "^\\./" filename)
			  '("./")
			(if (file-name-absolute-p filename)
			    '(nil)
			  (if Info-additional-directory-list
			      (append Info-directory-list
				      Info-additional-directory-list)
			    Info-directory-list)))))
	    (while (and dirs (not found))
	      (setq temp (expand-file-name filename (car dirs)))
	      (setq temp-downcase
		    (expand-file-name (downcase filename) (car dirs)))
	      (let ((suffix-list Info-suffix-list))
		(while (and suffix-list (not found))
		  (cond ((file-exists-p
			  (info-insert-file-contents-1
			   temp (car (car suffix-list))))
			 (setq found temp))
			((file-exists-p
			  (info-insert-file-contents-1
			   temp-downcase (car (car suffix-list))))
			 (setq found temp-downcase)))
		  (setq suffix-list (cdr suffix-list))))
	      (setq dirs (cdr dirs)))))
	(if found
	    (setq filename found)
	  (Octave-Info-error "Octave-Info file %s does not exist" filename))))
  (if (and Octave-Info-current-file (not no-going-back))
      (setq Octave-Info-history
	    (cons (list Octave-Info-current-file
			Octave-Info-current-node (point))
		  Octave-Info-history)))
  (switch-to-buffer "*octave-info*")
  (buffer-disable-undo (current-buffer))
  (or (eq major-mode 'Octave-Info-mode)
      (Octave-Info-mode))
  (widen)
  (setq Octave-Info-current-node nil)
  (unwind-protect
      (progn
	(or (null filename)
	    (equal Octave-Info-current-file filename)
	    (let ((buffer-read-only nil))
	      (setq Octave-Info-current-file nil
		    Octave-Info-current-subfile nil
		    Octave-Info-current-file-completions nil
		    Octave-Info-index-alternatives nil
		    buffer-file-name nil)
	      (erase-buffer)
	      (if (eq filename t)
		  (Octave-Info-insert-dir)
		(info-insert-file-contents filename t)
		(setq default-directory (file-name-directory filename)))
	      (set-buffer-modified-p nil)
	      (set-marker Octave-Info-tag-table-marker nil)
	      (goto-char (point-max))
	      (forward-line -8)
	      (or (string-equal nodename "*")
		  (not (search-forward "\^_\nEnd tag table\n" nil t))
		  (let (pos)
		    (search-backward "\nTag table:\n")
		    (setq pos (point))
		    (if (save-excursion
			  (forward-line 2)
			  (looking-at "(Indirect)\n"))
			(save-excursion
			  (let ((buf (current-buffer)))
			    (set-buffer
			     (get-buffer-create " *octave-info tag table*"))
                            (buffer-disable-undo (current-buffer))
			    (setq case-fold-search t)
			    (erase-buffer)
			    (insert-buffer-substring buf)
			    (set-marker Octave-Info-tag-table-marker
					(match-end 0))))
		      (set-marker Octave-Info-tag-table-marker pos))))
	      (setq Octave-Info-current-file
		    (if (eq filename t) "dir" filename))))
	(if (string-equal nodename "*")
	    (progn (setq Octave-Info-current-node nodename)
		   (Octave-Info-set-mode-line))
	  (let ((guesspos (point-min))
		(regexp (concat "Node: *"
				(regexp-quote nodename) " *[,\t\n\177]")))
	    (if (marker-position Octave-Info-tag-table-marker)
		(save-excursion
		  (set-buffer (marker-buffer Octave-Info-tag-table-marker))
		  (goto-char Octave-Info-tag-table-marker)
		  (if (re-search-forward regexp nil t)
		      (progn
			(setq guesspos (read (current-buffer)))
			(if (not (eq (current-buffer)
				     (get-buffer "*octave-info*")))
			    (setq guesspos
				  (Octave-Info-read-subfile guesspos))))
		    (Octave-Info-error "No such node: \"%s\"" nodename))))
	    (goto-char (max (point-min) (- guesspos 1000)))
	    (catch 'foo
	      (while (search-forward "\n\^_" nil t)
		(forward-line 1)
		(let ((beg (point)))
		  (forward-line 1)
		  (if (re-search-backward regexp beg t)
		      (throw 'foo t))))
	      (Octave-Info-error "No such node: %s" nodename)))
	  (Octave-Info-select-node)))
    (or Octave-Info-current-node no-going-back (null Octave-Info-history)
	(let ((hist (car Octave-Info-history)))
	  (setq Octave-Info-history (cdr Octave-Info-history))
	  (Octave-Info-find-node (nth 0 hist) (nth 1 hist) t)
	  (goto-char (nth 2 hist)))))
  (goto-char (point-min)))

(defvar Octave-Info-dir-contents nil)

(defvar Octave-Info-dir-contents-directory nil)

(defvar Octave-Info-dir-file-attributes nil)

(defun Octave-Info-insert-dir ()
  (if (and Octave-Info-dir-contents Octave-Info-dir-file-attributes
	   (eval (cons 'and
		       (mapcar '(lambda (elt)
				  (let ((curr (file-attributes (car elt))))
				    (if curr (setcar (nthcdr 4 curr) 0))
				    (setcar (nthcdr 4 (cdr elt)) 0)
				    (equal (cdr elt) curr)))
			       Octave-Info-dir-file-attributes))))
      (insert Octave-Info-dir-contents)
    (let ((dirs Info-directory-list)
	  buffers buffer others nodes dirs-done)
      (setq Octave-Info-dir-file-attributes nil)
      (while dirs
	(let ((truename (file-truename (expand-file-name (car dirs)))))
	  (or (member truename dirs-done)
	      (member (directory-file-name truename) dirs-done)
	      (let* (file
		     (attrs
		      (or
		       (progn
			 (setq file (expand-file-name "dir" truename))
			 (file-attributes file))
		       (progn
			 (setq file (expand-file-name "DIR" truename))
			 (file-attributes file))
		       (progn
			 (setq file (expand-file-name "dir.info" truename))
			 (file-attributes file))
		       (progn
			 (setq file (expand-file-name "DIR.INFO" truename))
			 (file-attributes file)))))
		(setq dirs-done
		      (cons truename
			    (cons (directory-file-name truename) dirs-done)))
		(if attrs
		    (save-excursion
		      (or buffers
			  (message "Composing main Octave-Info directory..."))
		      (set-buffer (generate-new-buffer "octave-info dir"))
		      (insert-file-contents file)
		      (setq buffers (cons (current-buffer) buffers)
			    Octave-Info-dir-file-attributes
			    (cons (cons file attrs)
				  Octave-Info-dir-file-attributes))))))
	  (setq dirs (cdr dirs))))
      
      (or buffers (Octave-Info-error "Can't find the octave-info directory node"))
      (setq buffer (car buffers) others (cdr buffers))

      (insert-buffer buffer)
      (setq Octave-Info-dir-contents-directory (save-excursion
						 (set-buffer buffer)
						 default-directory))
      
      (while others
	(let ((other (car others)))
	  (save-excursion
	    (set-buffer other)
	    (goto-char (point-min))
	    (while (re-search-forward "^\\* Menu:" nil t)
	      (let (beg nodename end)
		(forward-line 1)
		(setq beg (point))
		(search-backward "\n\^_")
		(search-forward "Node: ")
		(setq nodename (Info-following-node-name))
		(search-forward "\n\^_" nil 'move)
		(beginning-of-line)
		(setq end (point))
		(setq nodes (cons (list nodename other beg end) nodes))))))
	(setq others (cdr others)))
      (re-search-forward "^\\* Menu:")
      (forward-line 1)
      (let ((menu-items '("top"))
	    (nodes nodes)
	    (case-fold-search t)
	    (end (save-excursion (search-forward "\^_" nil t) (point))))
	(while nodes
	  (let ((nodename (car (car nodes))))
	    (save-excursion
	      (or (member (downcase nodename) menu-items)
		  (re-search-forward (concat "^\\* "
					     (regexp-quote nodename)
					     "::")
				     end t)
		  (progn
		    (insert "* " nodename "::" "\n")
		    (setq menu-items (cons nodename menu-items))))))
	  (setq nodes (cdr nodes))))
      (while nodes
	(let ((nodename (car (car nodes))))
	  (goto-char (point-min))
	  (if (re-search-forward (concat "\n\^_.*\n.*Node: "
					 (regexp-quote nodename)
					 "[,\n\t]")
				 nil t)
	      (progn
		(search-forward "\n\^_" nil 'move)
		(beginning-of-line)
		(insert "\n"))
	    (goto-char (point-max))
	    (insert "\^_\nFile: dir\tNode: " nodename "\n\n* Menu:\n\n"))
	  (apply 'insert-buffer-substring (cdr (car nodes))))
	(setq nodes (cdr nodes)))
      (while buffers
	(kill-buffer (car buffers))
	(setq buffers (cdr buffers)))
      (message "Composing main Octave-Info directory...done"))
    (setq Octave-Info-dir-contents (buffer-string)))
  (setq default-directory Octave-Info-dir-contents-directory))

(defun Octave-Info-read-subfile (nodepos)
  (set-buffer (marker-buffer Octave-Info-tag-table-marker))
  (goto-char (point-min))
  (search-forward "\n\^_")
  (let (lastfilepos
	lastfilename)
    (forward-line 2)
    (catch 'foo
      (while (not (looking-at "\^_"))
	(if (not (eolp))
	    (let ((beg (point))
		  thisfilepos thisfilename)
	      (search-forward ": ")
	      (setq thisfilename  (buffer-substring beg (- (point) 2)))
	      (setq thisfilepos (read (current-buffer)))
	      (forward-line 1)
	      (if (> thisfilepos nodepos) (throw 'foo t))
	      (setq lastfilename thisfilename)
	      (setq lastfilepos thisfilepos))
	  (forward-line 1))))
    (set-buffer (get-buffer "*octave-info*"))
    (or (equal Octave-Info-current-subfile lastfilename)
	(let ((buffer-read-only nil))
	  (setq buffer-file-name nil)
	  (widen)
	  (erase-buffer)
	  (info-insert-file-contents lastfilename)
	  (set-buffer-modified-p nil)
	  (setq Octave-Info-current-subfile lastfilename)))
    (goto-char (point-min))
    (search-forward "\n\^_")
    (+ (- nodepos lastfilepos) (point))))

(defun Octave-Info-select-node ()
  (save-excursion
    (search-backward "\n\^_")
    (forward-line 2)
    (re-search-forward "Node:[ \t]*")
    (setq Octave-Info-current-node
	  (buffer-substring-no-properties (point)
					  (progn
					    (skip-chars-forward "^,\t\n")
					    (point))))
    (Octave-Info-set-mode-line)
    (beginning-of-line)
    (let (active-expression)
      (narrow-to-region (point)
			(if (re-search-forward "\n[\^_\f]" nil t)
			    (prog1
				(1- (point))
			      (if (looking-at "[\n\^_\f]*execute: ")
				  (progn
				    (goto-char (match-end 0))
				    (setq active-expression
					  (read (current-buffer))))))
			  (point-max)))
      (if Octave-Info-enable-active-nodes (eval active-expression))
      (if Octave-Info-fontify (Octave-Info-fontify-node))
      (run-hooks 'Octave-Info-selection-hook))))

(defun Octave-Info-set-mode-line ()
  (setq mode-line-buffer-identification
	(concat
	 "Octave-Info:  ("
	 (if Octave-Info-current-file
	     (file-name-nondirectory Octave-Info-current-file) "") ")"
	     (or Octave-Info-current-node ""))))

(defun Octave-Info-goto-node (nodename)
  "Go to octave-info node named NAME.  Give just NODENAME."
  (interactive (list (Octave-Info-read-node-name "Goto node: ")))
  (let (filename)
    (string-match "\\s *\\((\\s *\\([^\t)]*\\)\\s *)\\s *\\|\\)\\(.*\\)"
		  nodename)
    (setq filename (if (= (match-beginning 1) (match-end 1))
		       ""
		     (substring nodename (match-beginning 2) (match-end 2)))
	  nodename (substring nodename (match-beginning 3) (match-end 3)))
    (let ((trim (string-match "\\s *\\'" filename)))
      (if trim (setq filename (substring filename 0 trim))))
    (let ((trim (string-match "\\s *\\'" nodename)))
      (if trim (setq nodename (substring nodename 0 trim))))
    (if transient-mark-mode (deactivate-mark))
    (Octave-Info-find-node (if (equal filename "") nil filename)
			   (if (equal nodename "") "Top" nodename))))

(defun Octave-Info-read-node-name-1 (string predicate code)
  (let ((no-completion (and (> (length string) 0) (eq (aref string 0) ?\())))
    (cond ((eq code nil)
	   (if no-completion
	       string
	     (try-completion string completion-table predicate)))
	  ((eq code t)
	   (if no-completion
	       nil
	     (all-completions string completion-table predicate)))
	  ((eq code 'lambda)
	   (if no-completion
	       t
	     (assoc string completion-table))))))

(defun Octave-Info-read-node-name (prompt &optional default)
  (let* ((completion-ignore-case t)
	 (completion-table (Octave-Info-build-node-completions))
	 (nodename (completing-read prompt 'Octave-Info-read-node-name-1)))
    (if (equal nodename "")
	(or default
	    (Octave-Info-read-node-name prompt))
      nodename)))

(defun Octave-Info-build-node-completions ()
  (or Octave-Info-current-file-completions
      (let ((compl nil))
	(save-excursion
	  (save-restriction
	    (if (marker-buffer Octave-Info-tag-table-marker)
		(progn
		  (set-buffer (marker-buffer Octave-Info-tag-table-marker))
		  (widen)
		  (goto-char Octave-Info-tag-table-marker)
		  (while (re-search-forward "\nNode: \\(.*\\)\177" nil t)
		    (setq compl
			  (cons (list (buffer-substring (match-beginning 1)
							(match-end 1)))
				compl))))
	      (widen)
	      (goto-char (point-min))
	      (while (search-forward "\n\^_" nil t)
		(forward-line 1)
		(let ((beg (point)))
		  (forward-line 1)
		  (if (re-search-backward "Node: *\\([^,\n]*\\) *[,\n\t]"
					  beg t)
		      (setq compl 
			    (cons (list (buffer-substring (match-beginning 1)
							  (match-end 1)))
				  compl))))))))
	(setq Octave-Info-current-file-completions compl))))

(defun Octave-Info-restore-point (hl)
  "If this node has been visited, restore the point value when we left."
  (while hl
    (if (and (equal (nth 0 (car hl)) Octave-Info-current-file)
	     (string-equal (nth 1 (car hl)) Octave-Info-current-node))
	(progn
	  (goto-char (nth 2 (car hl)))
	  (setq hl nil))
      (setq hl (cdr hl)))))

(defvar Octave-Info-last-search nil)

(defun Octave-Info-search (regexp)
  "Search for REGEXP, starting from point, and select node it's found in."
  (interactive "sSearch (regexp): ")
  (if transient-mark-mode (deactivate-mark))
  (if (equal regexp "")
      (setq regexp Octave-Info-last-search)
    (setq Octave-Info-last-search regexp))
  (let ((found ()) current
	(onode Octave-Info-current-node)
	(ofile Octave-Info-current-file)
	(opoint (point))
	(osubfile Octave-Info-current-subfile))
    (save-excursion
      (save-restriction
	(widen)
	(if (null Octave-Info-current-subfile)
	    (progn (re-search-forward regexp) (setq found (point)))
	  (condition-case err
	      (progn (re-search-forward regexp) (setq found (point)))
	    (search-failed nil)))))
    (if (not found)
	(unwind-protect
	    (let ((list ()))
	      (set-buffer (marker-buffer Octave-Info-tag-table-marker))
	      (goto-char (point-min))
	      (search-forward "\n\^_\nIndirect:")
	      (save-restriction
		(narrow-to-region (point)
				  (progn (search-forward "\n\^_")
					 (1- (point))))
		(goto-char (point-min))
		(search-forward (concat "\n" osubfile ": "))
		(beginning-of-line)
		(while (not (eobp))
		  (re-search-forward "\\(^.*\\): [0-9]+$")
		  (goto-char (+ (match-end 1) 2))
		  (setq list (cons (cons (read (current-buffer))
					 (buffer-substring (match-beginning 1)
							   (match-end 1)))
				   list))
		  (goto-char (1+ (match-end 0))))
		(setq list (nreverse list)
		      current (car (car list))
		      list (cdr list)))
	      (while list
		(message "Searching subfile %s..." (cdr (car list)))
		(Octave-Info-read-subfile (car (car list)))
		(setq list (cdr list))
		(if (re-search-forward regexp nil t)
		    (setq found (point) list ())))
	      (if found
		  (message "")
		(signal 'search-failed (list regexp))))
	  (if (not found)
	      (progn (Octave-Info-read-subfile opoint)
		     (goto-char opoint)
		     (Octave-Info-select-node)))))
    (widen)
    (goto-char found)
    (Octave-Info-select-node)
    (or (and (string-equal onode Octave-Info-current-node)
	     (equal ofile Octave-Info-current-file))
	(setq Octave-Info-history (cons (list ofile onode opoint)
					Octave-Info-history)))))

(defun Octave-Info-next ()
  "Go to the next node of this node."
  (interactive)
  (Octave-Info-goto-node (Info-extract-pointer "next")))

(defun Octave-Info-prev ()
  "Go to the previous node of this node."
  (interactive)
  (Octave-Info-goto-node (Info-extract-pointer "prev[ious]*" "previous")))

(defun Octave-Info-up ()
  "Go to the superior node of this node."
  (interactive)
  (Octave-Info-goto-node (Info-extract-pointer "up"))
  (Octave-Info-restore-point Octave-Info-history))

(defun Octave-Info-last ()
  "Go back to the last node visited."
  (interactive)
  (or Octave-Info-history
      (Octave-Info-error "This is the first Octave-Info node you looked at"))
  (let (filename nodename opoint)
    (setq filename (car (car Octave-Info-history)))
    (setq nodename (car (cdr (car Octave-Info-history))))
    (setq opoint (car (cdr (cdr (car Octave-Info-history)))))
    (setq Octave-Info-history (cdr Octave-Info-history))
    (Octave-Info-find-node filename nodename)
    (setq Octave-Info-history (cdr Octave-Info-history))
    (goto-char opoint)))

(defun Octave-Info-directory ()
  "Go to the Octave-Info directory node."
  (interactive)
  (Octave-Info-find-node "octave" "top"))

(defun Octave-Info-follow-reference (footnotename)
  "Follow cross reference named NAME to the node it refers to.
NAME may be an abbreviation of the reference name."
  (interactive
   (let ((completion-ignore-case t)
	 completions default alt-default (start-point (point)) str i bol eol)
     (save-excursion
       (end-of-line)
       (setq eol (point))
       (beginning-of-line)
       (setq bol (point))
       (goto-char (point-min))
       (while (re-search-forward "\\*note[ \n\t]*\\([^:]*\\):" nil t)
	 (setq str (buffer-substring
		    (match-beginning 1)
		    (1- (point))))
	 ;; See if this one should be the default.
	 (and (null default)
	      (<= (match-beginning 0) start-point)
	      (<= start-point (point))
	      (setq default t))
	 ;; See if this one should be the alternate default.
	 (and (null alt-default)
	      (and (<= bol (match-beginning 0))
		   (<= (point) eol))
	      (setq alt-default t))
	 (setq i 0)
	 (while (setq i (string-match "[ \n\t]+" str i))
	   (setq str (concat (substring str 0 i) " "
			     (substring str (match-end 0))))
	   (setq i (1+ i)))
	 ;; Record as a completion and perhaps as default.
	 (if (eq default t) (setq default str))
	 (if (eq alt-default t) (setq alt-default str))
	 (setq completions
	       (cons (cons str nil)
		     completions))))
     ;; If no good default was found, try an alternate.
     (or default
	 (setq default alt-default))
     ;; If only one cross-reference found, then make it default.
     (if (eq (length completions) 1)
         (setq default (car (car completions))))
     (if completions
	 (let ((input (completing-read (if default
					   (concat "Follow reference named: ("
						   default ") ")
					 "Follow reference named: ")
				       completions nil t)))
	   (list (if (equal input "")
		     default input)))
       (Octave-Info-error "No cross-references in this node"))))
  (let (target beg i (str (concat "\\*note " (regexp-quote footnotename))))
    (while (setq i (string-match " " str i))
      (setq str (concat (substring str 0 i) "[ \t\n]+" (substring str (1+ i))))
      (setq i (+ i 6)))
    (save-excursion
      (goto-char (point-min))
      (or (re-search-forward str nil t)
	  (Octave-Info-error "No cross-reference named %s" footnotename))
      (goto-char (+ (match-beginning 0) 5))
      (setq target
	    (Info-extract-menu-node-name "Bad format cross reference" t)))
    (while (setq i (string-match "[ \t\n]+" target i))
      (setq target (concat (substring target 0 i) " "
			   (substring target (match-end 0))))
      (setq i (+ i 1)))
    (Octave-Info-goto-node target)))

(defun Octave-Info-complete-menu-item (string predicate action)
  (let ((case-fold-search t))
    (cond ((eq action nil)
	   (let (completions
		 (pattern (concat "\n\\* \\("
				  (regexp-quote string)
				  "[^:\t\n]*\\):")))
	     (save-excursion
	       (set-buffer Octave-Info-complete-menu-buffer)
	       (goto-char (point-min))
	       (search-forward "\n* Menu:")
	       (while (re-search-forward pattern nil t)
		 (setq completions (cons (cons (format "%s"
						       (buffer-substring
							(match-beginning 1)
							(match-end 1)))
					       (match-beginning 1))
					 completions))))
	     (try-completion string completions predicate)))
	  ((eq action t)
	   (let (completions
		 (pattern (concat "\n\\* \\("
				  (regexp-quote string)
				  "[^:\t\n]*\\):")))
	     (save-excursion
	       (set-buffer Octave-Info-complete-menu-buffer)
	       (goto-char (point-min))
	       (search-forward "\n* Menu:")
	       (while (re-search-forward pattern nil t)
		 (setq completions (cons (cons (format "%s"
						       (buffer-substring
							(match-beginning 1)
							(match-end 1)))
					       (match-beginning 1))
					 completions))))
	     (all-completions string completions predicate)))
	  (t
	   (save-excursion
	     (set-buffer Octave-Info-complete-menu-buffer)
	     (goto-char (point-min))
	     (search-forward "\n* Menu:")
	     (re-search-forward (concat "\n\\* "
					(regexp-quote string)
					":")
				nil t))))))

(defun Octave-Info-menu (menu-item)
  "Go to node for menu item named (or abbreviated) NAME.
Completion is allowed, and the menu item point is on is the default."
  (interactive
   (let ((completions '())
	 (default nil)
	 (p (point))
	 beg
	 (last nil))
     (save-excursion
       (goto-char (point-min))
       (if (not (search-forward "\n* menu:" nil t))
	   (Octave-Info-error "No menu in this node"))
       (setq beg (point))
       (and (< (point) p)
	    (save-excursion
	      (goto-char p)
	      (end-of-line)
	      (re-search-backward "\n\\* \\([^:\t\n]*\\):" beg t)
	      (setq default (format "%s" (buffer-substring
					  (match-beginning 1)
					  (match-end 1)))))))
     (let ((item nil))
       (while (null item)
	 (setq item (let ((completion-ignore-case t)
			  (Octave-Info-complete-menu-buffer (current-buffer)))
		      (completing-read (if default
					   (format "Menu item (default %s): "
						   default)
					 "Menu item: ")
				       'Octave-Info-complete-menu-item nil t)))
	 (if (string= item "")
	     (if default
		 (setq item default)
	       (setq item nil))))
       (list item))))
  (Octave-Info-goto-node (Info-extract-menu-item menu-item)))

(defun Octave-Info-nth-menu-item ()
  "Go to the node of the Nth menu item.
N is the digit argument used to invoke this command."
  (interactive)
  (Octave-Info-goto-node
   (Info-extract-menu-counting
    (- (aref (this-command-keys) (1- (length (this-command-keys)))) ?0))))

(defun Octave-Info-top-node ()
  "Go to the Top node of this file."
  (interactive)
  (Octave-Info-goto-node "Top"))

(defun Octave-Info-final-node ()
  "Go to the final node in this file."
  (interactive)
  (Octave-Info-goto-node "Top")
  (let (Octave-Info-history)
    (Octave-Info-goto-node (Info-extract-menu-counting nil))
    (while (Octave-Info-forward-node t t) nil)
    (while (and (not (string-match "\\<index\\>" Octave-Info-current-node))
		(save-excursion (search-forward "\n* Menu:" nil t)))
      (Octave-Info-goto-node (Info-extract-menu-counting nil)))))

(defun Octave-Info-forward-node (&optional not-down no-error)
  "Go forward one node, considering all nodes as forming one sequence."
  (interactive)
  (goto-char (point-min))
  (forward-line 1)
  (cond ((and (not not-down)
              (save-excursion (search-forward "\n* menu:" nil t))
	      (not (string-match "\\<index\\>" Octave-Info-current-node)))
	 (Octave-Info-goto-node (Info-extract-menu-counting 1)) t)
        ((save-excursion (search-backward "next:" nil t))
         (Octave-Info-next) t)
        ((and (save-excursion (search-backward "up:" nil t))
	      (not (string-equal (downcase (Info-extract-pointer "up"))
				 "top")))
         (let ((old-node Octave-Info-current-node))
           (Octave-Info-up)
           (let (Octave-Info-history success)
             (unwind-protect
                 (setq success (Octave-Info-forward-node t no-error))
               (or success (Octave-Info-goto-node old-node))))))
        (no-error nil)
        (t (Octave-Info-error "No pointer forward from this node"))))

(defun Octave-Info-backward-node ()
  "Go backward one node, considering all nodes as forming one sequence."
  (interactive)
  (let ((prevnode (Info-extract-pointer "prev[ious]*" t))
	(upnode (Info-extract-pointer "up" t)))
    (cond ((and upnode (string-match "(" upnode))
	   (Octave-Info-error "First node in file"))
	  ((and upnode (or (null prevnode)
			   (string-equal (downcase prevnode)
					 (downcase upnode))))
	   (Octave-Info-up))
	  (prevnode
	   (Octave-Info-prev)
	   (let (Octave-Info-history)
	     (while (and (not (string-match "\\<index\\>"
					    Octave-Info-current-node))
			 (save-excursion (search-forward "\n* Menu:" nil t)))
	       (Octave-Info-goto-node (Info-extract-menu-counting nil)))))
	  (t (Octave-Info-error "No pointer backward from this node")))))

(defun Octave-Info-exit ()
  "Exit Octave-Info by selecting *octave* or some other buffer."
  (interactive)
  (if (equal Octave-Info-index-strings nil)
      (progn
	(split-window-vertically)
	(delete-window)
	(kill-buffer "*octave-info*")
	(if (and (get-buffer "*octave*") (get-buffer-process "*octave*"))
	    (let ((octave-mode-cb (current-buffer)))
	      (switch-to-buffer-other-window "*octave*")
	      (delete-window (get-buffer-window octave-mode-cb)))
	  (switch-to-buffer (prog1 (other-buffer (current-buffer))
			      (bury-buffer (current-buffer))))
	  (setq Octave-Info-running t)))
    (progn
      (Octave-Info-goto-node (format "(%s)" (car Octave-Info-index-strings)))
      (setq Octave-Info-index-strings (cdr Octave-Info-index-strings))
      (if (and (not (equal (car Octave-Info-index-strings) "(null)"))
	       (not (equal (car Octave-Info-index-strings) "")))
	  (Octave-Info-index (car Octave-Info-index-strings)))
      (setq Octave-Info-index-strings (cdr Octave-Info-index-strings)))))

(defun Octave-Info-next-menu-item ()
  (interactive)
  (save-excursion
    (forward-line -1)
    (search-forward "\n* menu:" nil t)
    (or (search-forward "\n* " nil t)
	(Octave-Info-error "No more items in menu"))
    (Octave-Info-goto-node (Info-extract-menu-node-name))))

(defun Octave-Info-last-menu-item ()
  (interactive)
  (save-excursion
    (forward-line 1)
    (let ((beg (save-excursion
		 (and (search-backward "\n* menu:" nil t) (point)))))
      (or (and beg (search-backward "\n* " beg t))
	  (Octave-Info-error "No previous items in menu")))
    (Octave-Info-goto-node (save-excursion (goto-char (match-end 0))
					   (Info-extract-menu-node-name)))))

(defun Octave-Info-next-preorder ()
  "Go to the next subnode or the next node, or go up a level."
  (interactive)
  (cond ((Info-no-error (Octave-Info-next-menu-item)))
	((Info-no-error (Octave-Info-next)))
	((Info-no-error (Octave-Info-up))
	 (goto-char (point-max)))
	(t (Octave-Info-error "No more nodes"))))

(defun Octave-Info-last-preorder ()
  "Go to the last node, popping up a level if there is none."
  (interactive)
  (cond ((Info-no-error
	  (Octave-Info-last-menu-item)
	  (goto-char (point-max)))
	 (recenter -1))
	((Info-no-error (Octave-Info-prev))
	 (goto-char (point-max))
	 (recenter -1))
	((Info-no-error (Octave-Info-up))
	 (goto-char (point-min))
	 (or (search-forward "\n* Menu:" nil t)
	     (goto-char (point-max))))
	(t (Octave-Info-error "No previous nodes"))))

(defun Octave-Info-scroll-up ()
  "Scroll one screenful forward in Octave-Info, considering all nodes as one 
sequence. Once you scroll far enough in a node that its menu appears on the 
screen, the next scroll moves into its first subnode.  When you scroll past
the end of a node, that goes to the next node or back up to the parent node."
  (interactive)
  (if (or (< (window-start) (point-min))
	  (> (window-start) (point-max)))
      (set-window-start (selected-window) (point)))
  (let ((virtual-end (save-excursion
		       (goto-char (point-min))
		       (if (search-forward "\n* Menu:" nil t)
			   (point)
			 (point-max)))))
    (if (or (< virtual-end (window-start))
	    (pos-visible-in-window-p virtual-end))
	(Octave-Info-next-preorder)
      (scroll-up))))

(defun Octave-Info-scroll-down ()
  "Scroll one screenful back in Octave-Info, considering all nodes as one 
sequence. Within the menu of a node, this goes to its last subnode.
When you scroll past the beginning of a node, that goes to the
previous node or back up to the parent node."
  (interactive)
  (if (or (< (window-start) (point-min))
	  (> (window-start) (point-max)))
      (set-window-start (selected-window) (point)))
  (let ((virtual-end (save-excursion
		       (goto-char (point-min))
		       (search-forward "\n* Menu:" nil t))))
    (if (or virtual-end (pos-visible-in-window-p (point-min)))
	(Octave-Info-last-preorder)
      (scroll-down))))

(defun Octave-Info-next-reference ()
  "Move cursor to the next cross-reference or menu item in the node."
  (interactive)
  (let ((pat "\\*note[ \n\t]*\\([^:]*\\):\\|^\\* .*:")
	(old-pt (point)))
    (or (eobp) (forward-char 1))
    (or (re-search-forward pat nil t)
	(progn
	  (goto-char (point-min))
	  (or (re-search-forward pat nil t)
	      (progn
		(goto-char old-pt)
		(Octave-Info-error "No cross references in this node")))))
    (goto-char (match-beginning 0))
    (if (looking-at "\\* Menu:")
	(Octave-Info-next-reference))))

(defun Octave-Info-prev-reference ()
  "Move cursor to the previous cross-reference or menu item in the node."
  (interactive)
  (let ((pat "\\*note[ \n\t]*\\([^:]*\\):\\|^\\* .*:")
	(old-pt (point)))
    (or (re-search-backward pat nil t)
	(progn
	  (goto-char (point-max))
	  (or (re-search-backward pat nil t)
	      (progn
		(goto-char old-pt)
		(Octave-Info-error "No cross references in this node")))))
    (goto-char (match-beginning 0))
    (if (looking-at "\\* Menu:")
	(Octave-Info-prev-reference))))

(defun Octave-Info-index (topic)
  "Look up a string in the index for this file.
The index is defined as the first node in the top-level menu whose
name contains the word \"Index\", plus any immediately following
nodes whose names also contain the word \"Index\".
If there are no exact matches to the specified topic, this chooses
the first match which is a case-insensitive substring of a topic.
Use the `,' command to see the other matches.
Give a blank topic name to go to the Index node itself."
  (interactive "sIndex topic: ")
  (let ((orignode Octave-Info-current-node)
	(rnode nil)
	(pattern (format "\n\\* \\([^\n:]*%s[^\n:]*\\):[ \t]*\\([^.\n]*\\)\\.[ \t]*\\([0-9]*\\)"
			 (regexp-quote topic)))
	node)
    (Octave-Info-goto-node "Top")
    (or (search-forward "\n* menu:" nil t)
	(Octave-Info-error "No index"))
    (or (re-search-forward "\n\\* \\(.*\\<Index\\>\\)" nil t)
	(Octave-Info-error "No index"))
    (goto-char (match-beginning 1))
    (let ((Octave-Info-history nil))
      (Octave-Info-goto-node (Info-extract-menu-node-name)))
    (or (equal topic "")
	(let ((matches nil)
	      (exact nil)
	      (Octave-Info-history nil)
	      found)
	  (while
	      (progn
		(goto-char (point-min))
		(while (re-search-forward pattern nil t)
		  (setq matches
			(cons (list (buffer-substring (match-beginning 1)
						      (match-end 1))
				    (buffer-substring (match-beginning 2)
						      (match-end 2))
				    Octave-Info-current-node
				    (string-to-int (concat "0"
							   (buffer-substring
							    (match-beginning 3)
							    (match-end 3)))))
			      matches)))
		(and (setq node (Info-extract-pointer "next" t))
		     (string-match "\\<Index\\>" node)))
	    (Octave-Info-goto-node node))
	  (or matches
	      (progn
		(Octave-Info-last)
		(Octave-Info-error "No \"%s\" in index" topic)))
	  (while (setq found (assoc topic matches))
	    (setq exact (cons found exact)
		  matches (delq found matches)))
	  (setq Octave-Info-index-alternatives
		(nconc exact (nreverse matches)))
	  (Octave-Info-index-next 0)))))

(defun Octave-Info-index-next (num)
  "Go to the next matching index item from the last `i' command."
  (interactive "p")
  (or Octave-Info-index-alternatives
      (Octave-Info-error "No previous `i' command in this file"))
  (while (< num 0)
    (setq num (+ num (length Octave-Info-index-alternatives))))
  (while (> num 0)
    (setq Octave-Info-index-alternatives
	  (nconc (cdr Octave-Info-index-alternatives)
		 (list (car Octave-Info-index-alternatives)))
	  num (1- num)))
  (Octave-Info-goto-node (nth 1 (car Octave-Info-index-alternatives)))
  (if (> (nth 3 (car Octave-Info-index-alternatives)) 0)
      (forward-line (nth 3 (car Octave-Info-index-alternatives)))
    (forward-line 3)
    (let ((name (car (car Octave-Info-index-alternatives))))
      (if (or (re-search-forward (format
				  "\\(Function\\|Command\\): %s\\( \\|$\\)"
				  (regexp-quote name)) nil t)
	      (search-forward (format "`%s'" name) nil t)
	      (and (string-match "\\`.*\\( (.*)\\)\\'" name)
		   (search-forward
		    (format "`%s'" (substring name 0 (match-beginning 1)))
		    nil t))
	      (search-forward name nil t))
	  (beginning-of-line)
	(goto-char (point-min)))))
  (message "Found \"%s\" in %s.  %s"
	   (car (car Octave-Info-index-alternatives))
	   (nth 2 (car Octave-Info-index-alternatives))
	   (if (cdr Octave-Info-index-alternatives)
	       "(Press `,' for more)"
	     "(Only match)")))

(defun Octave-Info-mouse-follow-nearest-node (click)
  "\\<Octave-Info-mode-map>Follow a node reference near point.
Like \\[Octave-Info-menu], \\[Octave-Info-follow-reference], \\[Octave-Info-next], \\[Octave-Info-prev] or \\[Octave-Info-up] command, depending on where you click.
At end of the node's text, moves to the next node, or up if none."
  (interactive "e")
  (let* ((start (event-start click))
	 (window (car start))
	 (pos (car (cdr start))))
    (select-window window)
    (goto-char pos))
  (and (not (Octave-Info-try-follow-nearest-node))
       (save-excursion (forward-line 1) (eobp))
       (Octave-Info-next-preorder)))

(defun Octave-Info-follow-nearest-node ()
  "\\<Octave-Info-mode-map>Follow a node reference near point.
Like \\[Octave-Info-menu], \\[Octave-Info-follow-reference], \\[Octave-Info-next], \\[Octave-Info-prev] or \\[Octave-Info-up] command, depending on where point is.
If no reference to follow, moves to the next node, or up if none."
  (interactive)
  (or (Octave-Info-try-follow-nearest-node)
      (Octave-Info-next-preorder)))

(defun Octave-Info-try-follow-nearest-node ()
  "Follow a node reference near point.  Return non-nil if successful."
  (let (node)
    (cond
     ((setq node (Info-get-token (point) "\\*note[ \n]"
				 "\\*note[ \n]\\([^:]*\\):"))
      (Octave-Info-follow-reference node))
     ((setq node (Info-get-token (point) "\\* " "\\* \\([^:]*\\)::"))
      (Octave-Info-goto-node node))
     ((setq node (Info-get-token (point) "\\* " "\\* \\([^:]*\\):"))
      (Octave-Info-menu node))
     ((setq node (Info-get-token (point) "Up: " "Up: \\([^,\n\t]*\\)"))
      (Octave-Info-goto-node node))
     ((setq node (Info-get-token (point) "Next: " "Next: \\([^,\n\t]*\\)"))
      (Octave-Info-goto-node node))
     ((setq node (Info-get-token (point) "File: " "File: \\([^,\n\t]*\\)"))
      (Octave-Info-goto-node "Top"))
     ((setq node (Info-get-token (point) "Prev: " "Prev: \\([^,\n\t]*\\)"))
      (Octave-Info-goto-node node)))
    node))

(defvar Octave-Info-mode-map nil
  "Keymap containing Octave-Info commands.")
(if Octave-Info-mode-map
    nil
  (setq Octave-Info-mode-map (make-keymap))
  (suppress-keymap Octave-Info-mode-map)
  (define-key Octave-Info-mode-map "." 'beginning-of-buffer)
  (define-key Octave-Info-mode-map " " 'Octave-Info-scroll-up)
  (define-key Octave-Info-mode-map "\C-m" 'Octave-Info-follow-nearest-node)
  (define-key Octave-Info-mode-map "\t" 'Octave-Info-next-reference)
  (define-key Octave-Info-mode-map "\e\t" 'Octave-Info-prev-reference)
  (define-key Octave-Info-mode-map "1" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "2" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "3" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "4" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "5" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "6" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "7" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "8" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "9" 'Octave-Info-nth-menu-item)
  (define-key Octave-Info-mode-map "0" 'undefined)
  (define-key Octave-Info-mode-map "]" 'Octave-Info-forward-node)
  (define-key Octave-Info-mode-map "[" 'Octave-Info-backward-node)
  (define-key Octave-Info-mode-map "<" 'Octave-Info-top-node)
  (define-key Octave-Info-mode-map ">" 'Octave-Info-final-node)
  (define-key Octave-Info-mode-map "b" 'beginning-of-buffer)
  (define-key Octave-Info-mode-map "d" 'Octave-Info-directory)
  (define-key Octave-Info-mode-map "f" 'Octave-Info-follow-reference)
  (define-key Octave-Info-mode-map "g" 'Octave-Info-goto-node)
  (define-key Octave-Info-mode-map "i" 'Octave-Info-index)
  (define-key Octave-Info-mode-map "l" 'Octave-Info-last)
  (define-key Octave-Info-mode-map "m" 'Octave-Info-menu)
  (define-key Octave-Info-mode-map "n" 'Octave-Info-next)
  (define-key Octave-Info-mode-map "p" 'Octave-Info-prev)
  (define-key Octave-Info-mode-map "q" 'Octave-Info-exit)
  (define-key Octave-Info-mode-map "s" 'Octave-Info-search)
  ;; For consistency with Rmail.
  (define-key Octave-Info-mode-map "\M-s" 'Octave-Info-search)
  (define-key Octave-Info-mode-map "t" 'Octave-Info-top-node)
  (define-key Octave-Info-mode-map "u" 'Octave-Info-up)
  (define-key Octave-Info-mode-map "," 'Octave-Info-index-next)
  (define-key Octave-Info-mode-map "\177" 'Octave-Info-scroll-down)
  (define-key Octave-Info-mode-map [mouse-2]
    'Octave-Info-mouse-follow-nearest-node)
  )

(put 'octave-info-mode 'mode-class 'special)

(defun Octave-Info-mode ()
  "See Info-mode for details."
  (kill-all-local-variables)
  (setq major-mode 'Octave-Info-mode)
  (setq mode-name "Octave-Info")
  (use-local-map Octave-Info-mode-map)
  (set-syntax-table text-mode-syntax-table)
  (setq local-abbrev-table text-mode-abbrev-table)
  (setq case-fold-search t)
  (setq buffer-read-only t)
  (make-local-variable 'Octave-Info-current-file)
  (make-local-variable 'Octave-Info-current-subfile)
  (make-local-variable 'Octave-Info-current-node)
  (make-local-variable 'Octave-Info-tag-table-marker)
  (make-local-variable 'Octave-Info-history)
  (make-local-variable 'Octave-Info-index-alternatives)
  (if (memq (framep (selected-frame)) '(x pc pm))
      (progn
	(make-face 'octave-info-node)
	(make-face 'octave-info-menu-5)
	(make-face 'octave-info-xref)
	(or (face-differs-from-default-p 'octave-info-node)
	    (if (face-differs-from-default-p 'bold-italic)
		(copy-face 'bold-italic 'octave-info-node)
	      (copy-face 'bold 'octave-info-node)))
	(or (face-differs-from-default-p 'octave-info-menu-5)
	    (set-face-underline-p 'octave-info-menu-5 t))
	(or (face-differs-from-default-p 'octave-info-xref)
	    (copy-face 'bold 'octave-info-xref)))
    (setq Octave-Info-fontify nil))
  (Octave-Info-set-mode-line)
  (run-hooks 'Octave-Info-mode-hook))

(defun Octave-Info-fontify-node ()
  (save-excursion
    (let ((buffer-read-only nil))
      (goto-char (point-min))
      (if (looking-at "^File: [^,: \t]+,?[ \t]+")
	  (progn
	    (goto-char (match-end 0))
	    (while
		(looking-at "[ \t]*[^:, \t\n]+:[ \t]+\\([^:,\t\n]+\\),?")
	      (goto-char (match-end 0))
	      (put-text-property (match-beginning 1) (match-end 1)
				 'face 'octave-info-xref)
	      (put-text-property (match-beginning 1) (match-end 1)
				 'mouse-face 'highlight))))
      (goto-char (point-min))
      (while (re-search-forward "\n\\([^ \t\n].+\\)\n\\(\\*+\\|=+\\|-+\\)$"
				nil t)
	(put-text-property (match-beginning 1) (match-end 1)
			   'face
			   (cdr (assq (preceding-char) Info-title-face-alist)))
	(put-text-property (match-end 1) (match-end 2)
			   'invisible t))
      (goto-char (point-min))
      (while (re-search-forward "\\*Note[ \n\t]+\\([^:]*\\):" nil t)
	(if (= (char-after (1- (match-beginning 0))) ?\")
	    nil
	  (put-text-property (match-beginning 1) (match-end 1)
			     'face 'octave-info-xref)
	  (put-text-property (match-beginning 1) (match-end 1)
			     'mouse-face 'highlight)))
      (goto-char (point-min))
      (if (and (search-forward "\n* Menu:" nil t)
	       (not (string-match "\\<Index\\>" Octave-Info-current-node))
	       (< (- (point-max) (point))
		  Octave-Info-fontify-maximum-menu-size))
	  (let ((n 0))
	    (while (re-search-forward "^\\* \\([^:\t\n]*\\):" nil t)
	      (setq n (1+ n))
	      (if (memq n '(5 9))
		  (put-text-property (match-beginning 0)
				     (1+ (match-beginning 0))
				     'face 'octave-info-menu-5))
	      (put-text-property (match-beginning 1) (match-end 1)
				 'face 'octave-info-node)
	      (put-text-property (match-beginning 1) (match-end 1)
				 'mouse-face 'highlight))))
      (set-buffer-modified-p nil))))

(provide 'octave-info)

;;; octave-info.el ends here
