;; octave-tmp-mode.el - major mode for Octave's temporary files
;;
;; (c) 1995 - 1997, Klaus Gebhardt

(require 'server)
(require 'octave)

;; Mode Map
(defvar octave-tmp-mode-map nil)
(setq octave-tmp-mode-map nil)
(cond ((not octave-tmp-mode-map)
       (setq octave-tmp-mode-map
	     (nconc (make-sparse-keymap) text-mode-map))
       (define-key octave-tmp-mode-map "\C-x#" 'octave-tmp-mode-server-edit)))

;; tmp-mode
(defun octave-tmp-mode ()
  "Major Mode for editing Octave's temporary files."
  (interactive)
  (text-mode)
  (setq major-mode 'Octave-tmp-mode)
  (setq mode-name "Octave-tmp")
  (use-local-map octave-tmp-mode-map)
  (make-local-variable 'backup-inhibited)
  (setq backup-inhibited t)
  (make-local-variable 'server-done-hook)
  (setq server-done-hook 'octave-tmp-buffer-done))

(defun octave-tmp-buffer-done ()
  "kill the temporary buffer."
  (interactive)
  (kill-buffer (current-buffer)))

(defun octave-tmp-mode-server-edit (&optional arg)
  "Switch to next server editing buffer; say \"Done\" for current buffer.
If a server buffer is current, it is marked \"done\" and optionally saved.
When all of a client's buffers are marked as \"done\", the client is notified."
  (interactive "P")
  (if (or arg
	  (not server-process)
	  (memq (process-status server-process) '(signal exit)))
      (server-start nil)
    (apply 'octave-tmp-server-switch-buffer (server-done))))

(defun octave-tmp-server-switch-buffer (&optional next-buffer killed-one)
  "Switch to another buffer, preferably one that has a client.
Arg NEXT-BUFFER is a suggestion; if it is a live buffer, use it."
  (cond ((and (windowp server-window)
	      (window-live-p server-window))
	 (select-window server-window))
	((framep server-window)
	 (if (not (frame-live-p server-window))
	     (setq server-window (make-frame)))
	 (select-window (frame-selected-window server-window))))
  (if (window-minibuffer-p (selected-window))
      (select-window (next-window nil 'nomini 0)))
  (let ((last-window (previous-window nil 'nomini 0)))
    (while (and (window-dedicated-p (selected-window))
		(not (eq last-window (selected-window))))
      (select-window (next-window nil 'nomini 0))))
  (set-window-dedicated-p (selected-window) nil)
  (if next-buffer
      (if (and (bufferp next-buffer)
	       (buffer-name next-buffer))
	  (switch-to-buffer next-buffer)
	(apply 'octave-tmp-server-switch-buffer
	       (server-buffer-done next-buffer)))
    (if server-clients
	(octave-tmp-server-switch-buffer
	 (nth 1 (car server-clients)) killed-one)
      (if (not killed-one)
	  (if (and (get-buffer "*octave*")
		   (get-buffer-process "*octave*"))
	      (switch-to-buffer "*octave*")
	    (switch-to-buffer (other-buffer)))))))

(provide 'octave-tmp-mode)

