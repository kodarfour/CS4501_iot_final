/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

 #include <zephyr/kernel.h>
 #include <zephyr/sys/reboot.h>
 
 #include <nfc_t2t_lib.h>
 #include <nfc/ndef/msg.h>
 #include <nfc/ndef/uri_msg.h>
 #include <dk_buttons_and_leds.h>
 
 #define NDEF_MSG_BUF_SIZE 128
 #define NFC_FIELD_LED     DK_LED1
 
 /* The form URL to open when scanned */
//  static const uint8_t uri[] = "https://forms.gle/abc123";

 static const uint8_t uri[] = "forms.gle/ti5AqpGLSvRMHVDA9";

 
 static uint8_t ndef_msg_buf[NDEF_MSG_BUF_SIZE];
 
 /* LED feedback on field presence */
 static void nfc_callback(void *context,
						  nfc_t2t_event_t event,
						  const uint8_t *data,
						  size_t data_length)
 {
	 ARG_UNUSED(context);
	 ARG_UNUSED(data);
	 ARG_UNUSED(data_length);
 
	 switch (event) {
	 case NFC_T2T_EVENT_FIELD_ON:
		 dk_set_led_on(NFC_FIELD_LED);
		 break;
	 case NFC_T2T_EVENT_FIELD_OFF:
		 dk_set_led_off(NFC_FIELD_LED);
		 break;
	 default:
		 break;
	 }
 }
 
 /** Encode a single URI record into the NDEF buffer */
 static int welcome_msg_encode(uint8_t *buffer, uint32_t *len)
 {
	 return nfc_ndef_uri_msg_encode(
		 NFC_URI_HTTPS,
		 uri,
		 sizeof(uri) - 1,
		 buffer,
		 len
	 );
 }
 
 void main(void)
 {
	 uint32_t len = sizeof(ndef_msg_buf);
 
	 printk("Starting NFC URI Tag ... \n");
 
	 /* Initialize LEDs */
	 if (dk_leds_init() < 0) {
		 printk("Failed to init LEDs\n");
		 goto fail;
	 }
 
	 /* Initialize NFC Type 2 Tag library */
	 if (nfc_t2t_setup(nfc_callback, NULL) < 0) {
		 printk("Failed to setup NFC T2T library\n");
		 goto fail;
	 }
 
	 /* Encode our URI into an NDEF message */
	 if (welcome_msg_encode(ndef_msg_buf, &len) < 0) {
		 printk("Failed to encode URI message\n");
		 goto fail;
	 }
 
	 /* Set that NDEF message as the tag’s payload */
	 if (nfc_t2t_payload_set(ndef_msg_buf, len) < 0) {
		 printk("Failed to set NFC payload\n");
		 goto fail;
	 }
 
	 /* Start emulating the tag — now phones can scan it! */
	 if (nfc_t2t_emulation_start() < 0) {
		 printk("Failed to start NFC emulation\n");
		 goto fail;
	 }
 
	 printk("NFC Tag ready — scan with your phone!\n");
 
	 while (1) {
		 k_sleep(K_FOREVER);
	 }
 
 fail:
 #if CONFIG_REBOOT
	 sys_reboot(SYS_REBOOT_COLD);
 #endif
 }
  