Veritrans UberCart (Drupal) Payment Gateway
=====================================

Let your Drupal UberCart store integrated with Veritrans payment gateway.

### Description

Veritrans payment gateway is an online payment gateway. They strive to make payments simple for both the merchant and customers. With this plugin you can allow online payment on your UberCart store using Veritrans payment gateway.

Payment Method Feature:

- **VT-Web** Payment

### Installation

#### Minimum Requirements

* Drupal 7.x or greater
* UberCart 3.x or greater
* PHP version 5.4 or greater
* MySQL version 5.0 or greater

#### Ubercart Module Dependencies

* uc_order
* uc_payment
* uc_cart

###### *These modules are by default included in your UberCart installation (no need to install/download separately).*

#### Manual Instalation

The manual installation method involves downloading our plugin and uploading it to your webserver via your FTP application.

1. Download the plugin file to your computer and unzip it
2. Rename the folder (that contain this readme and other files) to `ubercart_veritrans`
3. Using an FTP program, or your hosting control panel, upload the `ubercart_veritrans` folder to your Drupal installation's `/modules/` directory.
4. Make sure the structure look like this: `[Drupal folder]/modules/ubercart_veritrans/uc_veritrans.module (along with the other files).`
5. Activate the plugin from the `modules` menu within the Drupal admin page, look for `Ubercart Veritrans` module, and activate it. click `save configuration`.
  * Go to admin menu `Store > Configuration - Payment Method`, activate Veritrans, and click `settings`.
  * Insert your [Veritrans Acount](https://my.veritrans.co.id) server key in `Server Key` field
  * Select your `Payment mode`. Sandbox is for testing transaction, Production is for real transaction
  * You can leave other field as default, click `save configuration`.
6. Login to your [Veritrans Acount](https://my.veritrans.co.id), select your environment (sandbox/production), go to menu `settings > configuration`
  * Fill **Payment Notification URL** with `http://[your-web-domain]/vt_web/notify`
  * Fill **Finish Notification URL** with `http://[your-web-domain]/vt_web/success`
  * Fill **Unfinish Notification URL** with `http://[your-web-domain]/vt_web/unfinish`  
  * Fill **Error Notification URL** with `http://[your-web-domain]/vt_web/error`
  * Then **save** the configuration

Now you can make transaction in ubercart and will be integrated with Veritrans VtWeb.

#### Get help

* [Veritrans registration](https://my.veritrans.co.id/register)
* [Veritrans documentation](http://docs.veritrans.co.id)
* Technical support [support@veritrans.co.id](mailto:support@veritrans.co.id)
