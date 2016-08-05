<?php

/**
 * @file
 * Defines the veritrans pages.
 */

// Load Veritrans Library
require_once(dirname(__FILE__).'/veritrans/Veritrans.php');
require_once(dirname(__FILE__).'/veritrans/Veritrans/Notification.php');
require_once(dirname(__FILE__).'/veritrans/Veritrans/Transaction.php');

/**
 * Function to complete the transaction and sending request to Veritrans.
 */
function uc_veritrans_vtweb_payment_complete($order) {

  // Display an error if settings haven't been configured.
  $server_key = variable_get('uc_veritrans_server_key', '');
  if (empty($server_key)) {
    drupal_set_message(t('Veritrans payment method configuration isnt complete. Please check Veritrans Server Key.'), 'error');
    return FALSE;
  }

  // Set payment mode
  Veritrans_Config::$serverKey = variable_get('uc_veritrans_server_key', '');
  $payment_mode = variable_get('uc_veritrans_payment_mode', 'TEST');
  if ($payment_mode == 'TEST') {
    Veritrans_Config::$isProduction = FALSE;
  }
  else {
    Veritrans_Config::$isProduction = TRUE;
  }

  // Set 3DS setting
  $payment_mode = variable_get('uc_veritrans_3ds_mode', 'ON');
  if ($payment_mode == 'ON') {
    Veritrans_Config::$is3ds = TRUE;
  }
  else {
    Veritrans_Config::$is3ds = FALSE;
  }

  // build enabled payment method param
  // $list_enable_payments = array();
  // if (variable_get('uc_veritrans_pm_credit_card', TRUE) == TRUE)
  //   $list_enable_payments[] = "credit_card";
  // if (variable_get('uc_veritrans_pm_bank_transfer', TRUE) == TRUE)
  //   $list_enable_payments[] = "bank_transfer";
  // if (variable_get('uc_veritrans_pm_mandir_bill', TRUE) == TRUE)
  //   $list_enable_payments[] = "echannel";
  // if (variable_get('uc_veritrans_pm_mandiri_clickpay', TRUE) == TRUE)
  //   $list_enable_payments[] = "mandiri_clickpay";
  // if (variable_get('uc_veritrans_pm_cimb_clicks', TRUE) == TRUE)
  //   $list_enable_payments[] = "cimb_clicks";
  // if (variable_get('uc_veritrans_pm_bri_epay', TRUE) == TRUE)
  //   $list_enable_payments[] = "bri_epay";
  // if (variable_get('uc_veritrans_pm_telkomsel_cash', TRUE) == TRUE)
  //   $list_enable_payments[] = "telkomsel_cash";
  // if (variable_get('uc_veritrans_pm_xl_tunai', TRUE) == TRUE)
  //   $list_enable_payments[] = "xl_tunai";
  // if (variable_get('uc_veritrans_pm_indomaret', TRUE) == TRUE)
  //   $list_enable_payments[] = "cstore";
  // if (variable_get('uc_veritrans_pm_mandiri_ecash', TRUE) == TRUE)
  //   $list_enable_payments[] = "mandiri_ecash";

  // build item details param
  $items_details = array();
  foreach ($order->products as $item_id => $line_item) {
    $item = array();
    $item['id'] = $item_id;
    $item['quantity'] = $line_item->qty;
    $item['price'] = $line_item->price;
    $item['name'] = $line_item->title;
    $items_details[] = $item;
  }

  // set redirection urls param
  // $finish_redirect_url = variable_get('uc_veritrans_finish_redirect_url', 'vt_web/success');
  // $unfinish_redirect_url = variable_get('uc_veritrans_unfinish_redirect_url', 'vt_web/unfinish');
  // $error_redirect_url = variable_get('uc_veritrans_error_redirect_url', 'vt_web/error');

  // build vtweb param
  $params = array(
    "vtweb" => array(
      'credit_card_3d_secure' => variable_get('uc_veritrans_3ds_mode', 'ON') == 'ON' ? TRUE : FALSE,
      'finish_redirect_url' => url($finish_redirect_url, array('absolute' => TRUE)),
      'unfinish_redirect_url' => url($unfinish_redirect_url, array('absolute' => TRUE)),
      'error_redirect_url' => url($error_redirect_url, array('absolute' => TRUE)),
    ),
    'transaction_details' => array(
      'order_id' => $order->order_id,
      'gross_amount' => $order->order_total,
    ),
    'item_details' => $items_details,
    'customer_details' => array(
      'first_name' => $order->billing_first_name,
      'last_name' => $order->billing_last_name,
      'email' => $order->primary_email,
      'phone' => $order->billing_phone,
      'billing_address' => array(
        'first_name' => $order->billing_first_name,
        'last_name' => $order->billing_last_name,
        'address' => $order->billing_street1 . ' ' . $order->billing_street2,
        'city' => $order->billing_city,
        'postal_code' => $order->billing_postal_code,
        'country_code' => convert_country_code($order->billing_country),
        'phone' => $order->billing_phone,
      ),
    ),
  );

  try {
    // submit order
    uc_cart_complete_sale($order);

    // update order status to pending
    uc_order_update_status($order->order_id, 'veritrans_pending');

    // Get vtweb URL using param
    $path = Veritrans_VtWeb::getRedirectionUrl($params);

    // Redirect to url
    drupal_goto($path);
  }
  catch (Exception $e) {
    drupal_set_message(check_plain($e->getMessage()), 'error');
    return t('Something went wrong.');
  }
}

/**
 * Convert Country Code to ISO
 */
function convert_country_code($country_code) {

  $country_map = [4 => "AFG", 8 => "ALB", 10 => "ATA", 12 => "DZA", 16 => "ASM", 20 => "AND", 24 => "AGO", 28 => "ATG", 31 => "AZE", 32 => "ARG", 36 => "AUS", 40 => "AUT", 44 => "BHS", 48 => "BHR", 50 => "BGD", 51 => "ARM", 52 => "BRB", 56 => "BEL", 60 => "BMU", 64 => "BTN", 68 => "BOL", 70 => "BIH", 72 => "BWA", 74 => "BVT", 76 => "BRA", 84 => "BLZ", 86 => "IOT", 90 => "SLB", 92 => "VGB", 96 => "BRN", 100 => "BGR", 104 => "MMR", 108 => "BDI", 112 => "BLR", 116 => "KHM", 120 => "CMR", 124 => "CAN", 132 => "CPV", 136 => "CYM", 140 => "CAF", 144 => "LKA", 148 => "TCD", 152 => "CHL", 156 => "CHN", 158 => "TWN", 162 => "CXR", 166 => "CCK", 170 => "COL", 174 => "COM", 175 => "MYT", 178 => "COG", 180 => "COD", 184 => "COK", 188 => "CRI", 191 => "HRV", 192 => "CUB", 196 => "CYP", 203 => "CZE", 204 => "BEN", 208 => "DNK", 212 => "DMA", 214 => "DOM", 218 => "ECU", 222 => "SLV", 226 => "GNQ", 231 => "ETH", 232 => "ERI", 233 => "EST", 234 => "FRO", 238 => "FLK", 239 => "SGS", 242 => "FJI", 246 => "FIN", 248 => "ALA", 250 => "FRA", 254 => "GUF", 258 => "PYF", 260 => "ATF", 262 => "DJI", 266 => "GAB", 268 => "GEO", 270 => "GMB", 275 => "PSE", 276 => "DEU", 288 => "GHA", 292 => "GIB", 296 => "KIR", 300 => "GRC", 304 => "GRL", 308 => "GRD", 312 => "GLP", 316 => "GUM", 320 => "GTM", 324 => "GIN", 328 => "GUY", 332 => "HTI", 334 => "HMD", 336 => "VAT", 340 => "HND", 344 => "HKG", 348 => "HUN", 352 => "ISL", 356 => "IND", 360 => "IDN", 364 => "IRN", 368 => "IRQ", 372 => "IRL", 376 => "ISR", 380 => "ITA", 384 => "CIV", 388 => "JAM", 392 => "JPN", 398 => "KAZ", 400 => "JOR", 404 => "KEN", 408 => "PRK", 410 => "KOR", 414 => "KWT", 417 => "KGZ", 418 => "LAO", 422 => "LBN", 426 => "LSO", 428 => "LVA", 430 => "LBR", 434 => "LBY", 438 => "LIE", 440 => "LTU", 442 => "LUX", 446 => "MAC", 450 => "MDG", 454 => "MWI", 458 => "MYS", 462 => "MDV", 466 => "MLI", 470 => "MLT", 474 => "MTQ", 478 => "MRT", 480 => "MUS", 484 => "MEX", 492 => "MCO", 496 => "MNG", 498 => "MDA", 499 => "MNE", 500 => "MSR", 504 => "MAR", 508 => "MOZ", 512 => "OMN", 516 => "NAM", 520 => "NRU", 524 => "NPL", 528 => "NLD", 531 => "CUW", 533 => "ABW", 534 => "SXM", 535 => "BES", 540 => "NCL", 548 => "VUT", 554 => "NZL", 558 => "NIC", 562 => "NER", 566 => "NGA", 570 => "NIU", 574 => "NFK", 578 => "NOR", 580 => "MNP", 581 => "UMI", 583 => "FSM", 584 => "MHL", 585 => "PLW", 586 => "PAK", 591 => "PAN", 598 => "PNG", 600 => "PRY", 604 => "PER", 608 => "PHL", 612 => "PCN", 616 => "POL", 620 => "PRT", 624 => "GNB", 626 => "TLS", 630 => "PRI", 634 => "QAT", 638 => "REU", 642 => "ROU", 643 => "RUS", 646 => "RWA", 652 => "BLM", 654 => "SHN", 659 => "KNA", 660 => "AIA", 662 => "LCA", 663 => "MAF", 666 => "SPM", 670 => "VCT", 674 => "SMR", 678 => "STP", 682 => "SAU", 686 => "SEN", 688 => "SRB", 690 => "SYC", 694 => "SLE", 702 => "SGP", 703 => "SVK", 704 => "VNM", 705 => "SVN", 706 => "SOM", 710 => "ZAF", 716 => "ZWE", 724 => "ESP", 732 => "ESH", 736 => "SDN", 740 => "SUR", 744 => "SJM", 748 => "SWZ", 752 => "SWE", 756 => "CHE", 760 => "SYR", 762 => "TJK", 764 => "THA", 768 => "TGO", 772 => "TKL", 776 => "TON", 780 => "TTO", 784 => "ARE", 788 => "TUN", 792 => "TUR", 795 => "TKM", 796 => "TCA", 798 => "TUV", 800 => "UGA", 804 => "UKR", 807 => "MKD", 818 => "EGY", 826 => "GBR", 831 => "GGY", 832 => "JEY", 833 => "IMN", 834 => "TZA", 840 => "USA", 850 => "VIR", 854 => "BFA", 858 => "URY", 860 => "UZB", 862 => "VEN", 876 => "WLF", 882 => "WSM", 887 => "YEM", 894 => "ZMB"];

  // Set Indonesia as default if country is undefined
  $mapped_country_code = array_key_exists((int) $country_code, $country_map) ? $country_map[ (int) $country_code] : 'IDN';
  return $mapped_country_code;
}

/**
 * Success callback / landing page.
 */
function uc_veritrans_vt_web_payment_success() {
  return t('Thank you, your payment is now complete! We will process your order.');
}

/**
 * Unfinish callback / landing page.
 */
function uc_veritrans_vt_web_payment_unfinish() {
  return t('Your payment is not yet complete, please finish your payment as instructed before.');
}

/**
 * Error callback / landing page.
 */
function uc_veritrans_vt_web_payment_error() {
  return t('Payment error, please do re-checkout or try different payment method.');
}

/**
 * Instant Payment Notification for veritrans payment gateway.
 */
function uc_veritrans_vt_web_payment_notification() {
  // Set library config
  $veritrans = new Veritrans_Config();
  Veritrans_Config::$serverKey = variable_get('uc_veritrans_server_key', '');
  // error_log("Veritrans_Config::$serverKey --- ".Veritrans_Config::$serverKey);  // debugan
  // error_log("variable_get('uc_veritrans_server_key', '') --- ".variable_get('uc_veritrans_server_key', '')); // debugan
  $payment_mode = variable_get('uc_veritrans_payment_mode', 'TEST');
  if ($payment_mode == 'TEST') {
    Veritrans_Config::$isProduction = FALSE;
  }
  else {
    Veritrans_Config::$isProduction = TRUE;
  }

  // create notification object from HTTP POST received
  $notification = new Veritrans_Notification();

  // get order id
  $order_id = $notification->order_id;
  if (empty($order_id)) {
    $error_redirect_url = variable_get('uc_veritrans_error_redirect_url', 'vt_web/error');
    drupal_goto($error_redirect_url);
  }

  // build order object
  $order = uc_order_load($order_id);
  if (variable_get('uc_veritrans_debugging', FALSE)) {
    watchdog('vt_web', 'Response from Veritrance  <pre>@response</pre>', array('@response' => print_r($notification, TRUE)), WATCHDOG_DEBUG);
  }
  if ($order == FALSE) {
    watchdog('vt_web', 'IPN attempted for non-existent order @order_id.', array('@order_id' => $notification->order_id), WATCHDOG_ERROR);
    return FALSE;
  }

  // get the order status
  $result = Veritrans_Transaction::status($order_id);
  if ($result->transaction_id != $notification->transaction_id || $result->fraud_status != $notification->fraud_status) {
    watchdog('vt_web', 'Verification of transaction failed.', array(), WATCHDOG_WARNING);
    return FALSE;
  }

  // Do action according to transaction status
  $transaction = $notification->transaction_status;
  $fraud = $notification->fraud_status;
  if ($transaction == 'capture') {
    if ($fraud == 'challenge') {
      // Set payment status in merchant's database to 'challenge'.
      uc_order_update_status($order->order_id, 'veritrans_pending');
      uc_order_comment_save($order->order_id, 0, t('Payment is challanged, please take action in Veritrans Portal'), 'admin');
    }
    elseif ($fraud == 'accept') {
      // Set payment status in merchant's database to 'success'.
      $payment_message = t('Payment using Veritrans for ORDERNUMBER: @ordernumber.', array('@ordernumber' => $order->order_id));
      uc_payment_enter($order->order_id, 'vt_web', $order->order_total, $order->uid, NULL, $payment_message);

      uc_order_update_status($order->order_id, 'payment_received');
      $payment_reported = t('Veritrans  reported a payment of @amount.', array('@amount' => $order->order_total));
      uc_order_comment_save($order->order_id, 0, $payment_reported, 'admin');
    }
  }
  elseif ($notification->payment_type != 'credit_card' && $transaction == 'settlement') {
      // Set payment status in merchant's database to 'success'.
      $payment_message = t('Payment using Veritrans for ORDERNUMBER: @ordernumber.', array('@ordernumber' => $order->order_id));
      uc_payment_enter($order->order_id, 'vt_web', $order->order_total, $order->uid, NULL, $payment_message);

      uc_order_update_status($order->order_id, 'payment_received');
      $payment_reported = t('Veritrans  reported a payment of @amount.', array('@amount' => $order->order_total));
      uc_order_comment_save($order->order_id, 0, $payment_reported, 'admin');
  }
  elseif ($transaction == 'cancel') {
    if ($fraud == 'challenge' || $fraud == 'accept') {
      // Set payment status in merchant's database to 'failure'.
  	  uc_order_update_status($order->order_id, 'canceled');
      uc_order_comment_save($order->order_id, 0, t('Payment canceled by merchant.'), 'admin');
    }
  }
  elseif ($transaction == 'deny') {
    // Set payment status in merchant's database to 'failure'.
    uc_order_update_status($order->order_id, 'canceled');
    uc_order_comment_save($order->order_id, 0, t('Payment declined by merchant.'), 'admin');
  }
  elseif ($transaction == 'pending') {
    // Set payment status in merchant's database to 'pending'.
    uc_order_update_status($order->order_id, 'pending');
    uc_order_comment_save($order->order_id, 0, t('Awaiting customer payment at Veritrans.'), 'admin');
  }
  else {
    watchdog('vt_web', 'Invalid Transaction. Check Response from Veritrance <pre>@response</pre>', array('@response' => print_r($notification, TRUE)), WATCHDOG_DEBUG);
  }
}
