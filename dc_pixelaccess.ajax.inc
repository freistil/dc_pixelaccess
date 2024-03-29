<?php

/**
 * AJAX Menu Callback.
 */
function dc_pixelaccess_ajax_callback() {
  // Exit if nothing was passed to this.
	if (   !isset($_GET['nid'])
      && !isset($_GET['title'])
      && !isset($_GET['qq'])
      && !isset($_GET['referer'])
      ) {
    return drupal_not_found();
  }

  if (!isset($_GET['js'])) {
    // stats not called via JS, send image out & close connection.
    dc_pixelaccess_async_opp("\x47\x49\x46\x38\x39\x61\x1\x0\x1\x0\x80\xff\x0\xc0\xc0\xc0\x0\x0\x0\x21\xf9\x4\x1\x0\x0\x0\x0\x2c\x0\x0\x0\x0\x1\x0\x1\x0\x0\x2\x2\x44\x1\x0\x3b", TRUE, 'image/gif', 43);
  }

  // Set variables passed via GET.
  $nid = (isset($_GET['nid']) && is_numeric($_GET['nid'])) ? $_GET['nid'] : NULL;
  $title = (isset($_GET['title']) && $_GET['title'] != 'NULL') ? urldecode($_GET['title']) : NULL;
  $q = (isset($_GET['qq']) && $_GET['qq'] != 'NULL') ? $_GET['qq'] : NULL;
  $referer = isset($_GET['referer']) ? $_GET['referer'] : NULL;
	
  // $session_id only goes in the access log; only used for stats, not creds.
  $session_id = session_id();
  if (empty($session_id)) {
    if (empty($_COOKIE[session_name()])) {
      if (empty($_SERVER['HTTP_USER_AGENT'])) {
        $session_id = md5(ip_address());
      }
      else {
        $session_id = md5($_SERVER['HTTP_USER_AGENT'] . ip_address());
      }
    }
    else {
      $session_id = $_COOKIE[session_name()];
    }
  }

  // Anonymous users always get a User ID of 0.
  $uid = 0;

  // Set node counter.
  if (variable_get('statistics_count_content_views', FALSE)) {
    dc_pixelaccess_update_node_counter($nid);
  }

  // Set access log.
  if (variable_get('statistics_enable_access_log', FALSE)) {
    dc_pixelaccess_add_access_log($title, $q, $referer, $session_id, $uid);
  }

	// Return Data
	if (isset($_GET['js'])) {
		if ($_GET['js'] == 1) {
			$json = array("success" => "1");
			// Get stats block html.
			$json = array_merge($json);
			
			// Send JSON Back
			if (!empty($json)) {
				drupal_json($json);
			}
		}
		// Send HTML back
		elseif ($_GET['js'] == 2) {
			echo "success";
		}
	}
}

/**
 * Update database node count.
 *
 * @param $nid
 *  Node ID of page.
 */
function dc_pixelaccess_update_node_counter($nid) {
  // A node has been viewed, so update the node's counters.
  db_query('UPDATE {node_counter} SET daycount = daycount + 1, totalcount = totalcount + 1, timestamp = %d WHERE nid = %d', time(), $nid);
  // If we affected 0 rows, this is the first time viewing the node.
  if (!db_affected_rows()) {
    // We must create a new row to store counters for the new node.
    db_query('INSERT INTO {node_counter} (nid, daycount, totalcount, timestamp) VALUES (%d, 1, 1, %d)', $nid, time());
  }
}

/**
 * Add entry to access log
 *
 * @param $title
 *  Title of page.
 * @param $q
 *  URL directory structure
 * @param $referrer
 *  Javascript referrer
 * @param $session_id
 *  Session ID
 * @param $uid
 *  User ID; should be 0.
 */
function dc_pixelaccess_add_access_log($title, $q, $referer, $session_id, $uid) {
  db_query("INSERT INTO {accesslog} (title, path, url, hostname, uid, sid, timer, timestamp) values('%s', '%s', '%s', '%s', %d, '%s', %d, %d)", $title, $q, $referer, ip_address(), $uid, $session_id, timer_read('page'), time());
}

