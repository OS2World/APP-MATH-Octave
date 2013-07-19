;;; octave-cmd.el --- specialized comint.el for running the Octave.

;; Copyright (C) 1996 - 1997 Klaus Gebhardt

;; This file is part of Octave for OS/2.

;; Octave is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 2, or (at your option)
;; any later version.

;; Octave is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with Octave; see the file COPYING.  If not, write to
;; the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

;;; Code:

(require 'shell)
(require 'comint)
(require 'octave-info)

;; Constants used in all Octave-mode buffers.

(defvar octave-prog-filename "octave")
(defvar octave-temp-string nil)

(defvar octave-shell-prompt-regexp "^octave:[0-9]*> *")
(defvar octave-edit-regexp "\\(ans = \\)\\(.*.[mM]\\)$")
(defvar octave-info-regexp
  "\\(\\*\\*\\* Starting Emacs info browser (\\)\\(.*\\)): \\(.*\\)\\( \\*\\*\\*$\\)")

(if window-system
    (progn
      (require 'hilit19)
      (hilit-set-mode-patterns
       '(octave-shell-mode)
       (append
	'(("^warning.*$" nil comment))
	'(("^error.*$" nil comment))
	'(("^parse error.*$" nil comment))
	'(("^>>>.*$" nil comment))
	'(("^octave:[0-9]*>" nil define))
	'(("^ans =" nil keyword))
	'(("^ Columns .* through .*$" nil keyword))
	'(("^ Columns .* and .*$" nil keyword))
	'(("^Process octave finished$" nil comment))))))

(defvar octave-shell-mode-map nil)
(setq octave-shell-mode-map nil)
(cond ((not octave-shell-mode-map)
       (setq octave-shell-mode-map
	     (nconc (make-sparse-keymap) shell-mode-map))
       (define-key octave-shell-mode-map
	 [menu-bar octave] 
	 (cons "Octave" (make-sparse-keymap "Octave")))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-help]
	 '("Help" . octave-help))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-quit]
	 '("Quit" . octave-quit))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-start]
	 '("Restart" . octave))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting]
	 (cons "Plotting" (make-sparse-keymap "Plotting")))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-purgetemp]
	 '("Purge Tempfiles" . octave-purgetemp))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-subwindow]
	 '("MultiPlot Subwindow" . octave-subwindow))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-closemultiplot]
	 '("Close MultiPlot" . octave-closemultiplot))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-multiplot]
	 '("MultiPlot" . octave-multiplot))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-closeplot]
	 '("Close Plot" . octave-closeplot))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-replot]
	 '("Replot" . octave-replot))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set]
	 (cons "Set" (make-sparse-keymap "Set")))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set octave-zlabel]
	 '("Zlabel" . octave-zlabel))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set octave-ylabel]
	 '("Ylabel" . octave-ylabel))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set octave-xlabel]
	 '("Xlabel" . octave-xlabel))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set octave-title]
	 '("Title" . octave-title))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set octave-term]
	 '("Terminal" . octave-term))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-plotting-set octave-output]
	 '("Output" . octave-output))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-plotting octave-sombrero]
	 '("Sombrero" . octave-sombrero))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-system]
	 (cons "System" (make-sparse-keymap "System")))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-system octave-computer]
	 '("Computer" . octave-computer))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-system octave-warranty]
	 '("Warranty" . octave-warranty))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-system octave-dir]
	 '("Directory" . octave-dir))
       (define-key octave-shell-mode-map
	 [menu-bar octave octave-system octave-edit-script]
	 '("Edit Scriptfile" . octave-edit-script))))

(defun octave-send-string (string)
  (let ((proc (get-buffer-process (current-buffer))))
    (if (not proc)
	(error "Current buffer has no process")
      (comint-send-string proc string))))

(defun octave-read-from-process (proc output)
  (setq octave-temp-string (format "%s%s" octave-temp-string output)))

(defun octave-ask (string)
  (let ((proc (get-buffer-process (current-buffer))))
    (if (not proc)
	(progn
	  (error "Current buffer has no process")
	  (setq octave-temp-string ""))
      (progn
	(setq octave-temp-string "")
	(set-process-filter proc 'octave-read-from-process)
	(process-send-string proc "add_to_command_number(-1); ")
	(process-send-string proc string)
	(process-send-string proc "\n")
	(while (not (string-match octave-shell-prompt-regexp
				  octave-temp-string 0))
	  (accept-process-output proc))
	(set-process-filter proc 'octave-output-filter)))))

(defun octave-output-filter (process string)
  (let ((oprocbuf (process-buffer process)))
    (if (and oprocbuf (buffer-name oprocbuf))
	(let ((obuf (current-buffer))
	      (opoint nil) (obeg nil) (oend nil))

	  (setq octave-temp-string nil)
	  (while (string-match octave-info-regexp string 0)
	    (setq octave-temp-string
		  (cons (substring string (match-beginning 2) (match-end 2))
			octave-temp-string))
	    (setq octave-temp-string
		  (cons (substring string (match-beginning 3) (match-end 3))
			octave-temp-string))
	    (setq string (format
			  "%s%s"
			  (substring string 0 (match-beginning 1))
			  (substring string (+ (match-end 4) 1) nil))))

	  (set-buffer oprocbuf)
	  (setq opoint (point))
	  (setq obeg (point-min))
	  (setq oend (point-max))
	  (let ((buffer-read-only nil)
		(nchars (length string))
		(ostart nil))
	    (widen)
	    (goto-char (process-mark process))
	    (setq ostart (point))
	    (if (<= (point) opoint)
		(setq opoint (+ opoint nchars)))
	    (if (< (point) obeg)
		(setq obeg (+ obeg nchars)))
	    (if (<= (point) oend)
		(setq oend (+ oend nchars)))
	    (insert-before-markers string)
	    (if (= (window-start (selected-window)) (point))
		(set-window-start (selected-window)
				  (- (point) (length string))))
	    (if (and comint-last-input-end
		     (marker-buffer comint-last-input-end)
		     (= (point) comint-last-input-end))
		(set-marker comint-last-input-end
			    (- comint-last-input-end nchars)))
	    (set-marker comint-last-output-start ostart)
	    (set-marker (process-mark process) (point))
	    (force-mode-line-update)

	  (narrow-to-region obeg oend)
	  (goto-char opoint))
	  (run-hook-with-args 'comint-output-filter-functions string)

	  (if (not (equal octave-temp-string nil))
	      (progn
		(switch-to-buffer-other-window "*octave-info*")
		(octave-info (nreverse octave-temp-string))
		(switch-to-buffer-other-window "*octave*")))

	  (set-buffer obuf)))))

(defun my-comint-output-filter (string)
  (comint-postoutput-scroll-to-bottom string)
  (if window-system (hilit-rehighlight-buffer-quietly)))

(defun octave-zlabel (str)
  (interactive "sZlabel: ")
  (octave-send-string (format "gset zlabel \"%s\"\n" str)))

(defun octave-ylabel (str)
  (interactive "sYlabel: ")
  (octave-send-string (format "gset ylabel \"%s\"\n" str)))

(defun octave-xlabel (str)
  (interactive "sXlabel: ")
  (octave-send-string (format "gset xlabel \"%s\"\n" str)))

(defun octave-title (str)
  (interactive "sTitle: ")
  (octave-send-string (format "gset title \"%s\"\n" str)))

(defun octave-term (str)
  (interactive "sTerminal: ")
  (octave-send-string (format "gset term %s\n" str)))

(defun octave-output (str)
  (interactive "sOutput: ")
  (if (equal str "")
      (octave-send-string (format "gset output\n"))
    (octave-send-string (format "gset output \"%s\"\n" str))))

(defun octave-sombrero (num)
  (interactive "nResolution: ")
  (octave-send-string
   (format "sombrero (%d)\n" num)))

(defun octave-edit-script (script)
  (interactive "sScriptfile: ")
  (octave-ask (format "file_in_path(LOADPATH, \'%s.m\')" script))
  (if (string-match octave-edit-regexp octave-temp-string 0)
      (find-file-other-window
       (substring octave-temp-string (match-beginning 2) (match-end 2)))))

(defun octave-computer ()
  (interactive)
  (octave-send-string "computer\n"))

(defun octave-warranty ()
  (interactive)
  (octave-send-string "warranty\n"))

(defun octave-dir ()
  (interactive)
  (octave-send-string "ls -l\n"))

(defun octave-replot ()
  (interactive)
  (octave-send-string "replot\n"))

(defun octave-purgetemp ()
  (interactive)
  (octave-send-string "purge_tmp_files\n"))

(defun octave-closeplot ()
  (interactive)
  (octave-send-string "closeplot\n"))

(defun octave-subwindow (xn yn)
  (interactive "nxn: \nnyn: ")
  (octave-send-string (format "subwindow (%d, %d)\n" xn yn)))

(defun octave-closemultiplot ()
  (interactive)
  (octave-send-string "multiplot (0, 0)\n"))

(defun octave-multiplot (xn yn)
  (interactive "nxn: \nnyn: ")
  (octave-send-string (format "multiplot (%d, %d)\n" xn yn)))

(defun octave-quit ()
  (interactive)
  (octave-send-string "quit\n"))

(defun octave-help (index)
  (interactive "sIndex: ")
  (octave-send-string (format "help -i %s\n" index)))

(defun octave-shell-mode ()
  (interactive)
  (shell-mode)
  (setq major-mode 'octave-shell-mode)
  (setq mode-name "Octave")
  (use-local-map octave-shell-mode-map)
  (make-local-variable 'comint-prompt-regexp)
  (setq comint-prompt-regexp octave-shell-prompt-regexp)
  (make-local-variable 'comint-input-ring-file-name)
  (setq comint-input-ring-file-name "~/.octave_hist")
  (make-local-variable 'comint-process-echoes)
  (setq comint-process-echoes nil)
  (make-local-variable 'comint-scroll-to-bottom-on-input)
  (setq comint-scroll-to-bottom-on-input nil)
  (make-local-variable 'comint-scroll-to-bottom-on-output)
  (setq comint-scroll-to-bottom-on-output t)
  (make-local-variable 'comint-input-ignoredups)
  (setq comint-input-ignoredups nil)
  (make-local-variable 'comint-output-filter-functions)
  (setq comint-output-filter-functions 'my-comint-output-filter)
  (run-hooks 'octave-shell-mode-hook))

(defun octave ()
  "Run Octave 1.1.1 with I/O through buffer *octave*.
See shell for details."
  (interactive)
  (cond ((not (comint-check-proc "*octave*"))
	 (let* ((prog octave-prog-filename)
		(name (file-name-nondirectory prog))
		(startfile (concat "~/.emacs_" name)))
	   (set-buffer (apply 'make-comint "octave" prog
			      (if (file-exists-p startfile) startfile)
			      '("-I")))
	   (octave-shell-mode)
	   (set-process-filter (get-buffer-process "*octave*")
			       'octave-output-filter))))
  (switch-to-buffer "*octave*"))

(provide 'octave)

;;; octave-cmd.el ends here
