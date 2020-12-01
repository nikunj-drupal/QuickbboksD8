<?php 
/**
 * HTML Structure Handler
 */
class QuickBookInvoiceHTML {
	
	protected $data = array();

    /**
     * Constructer
     */
    public function __construct($invoiceId = '', $invoiceData = array(), $paymentTotal = '') {
        $this->data['invoice_id'] = $invoiceId;
        $this->data['invoice_data'] = $invoiceData;
        $this->data['invoice_total'] = $paymentTotal;
        $this->data['invoice_date'] = date('Y-m-d');
    }

    /**
	 * @param $invoice_id
	 * QuickBook Invoice Error Msg
	 * @return String
	 */
	public function quickbookInvoiceError() {
		$msg = '';
		if(!empty($this->data['invoice_id'])) {
			$msg = t("We are unable to process invoice @INV. Please contact to administrator.", array('@INV' => $this->data['invoice_id']));
		}
		else {
			$msg = 'Invoice ID is not found. Please contact to administrator.';
			watchdog("Payment error", '<pre>' . print_r( $msg, true) . '</pre>');
		}
		return $msg;
	}

	/**
	 * QuickBook Invoice Pamyent Success
	 */
	public function quickbookInvoicePaymentSuccess() {
		drupal_set_message('Your payment was successful. Thank You');
		global $base_url;
		$invoice_id = $this->data['invoice_id'];
		$payment_id = $this->data['invoice_data']['payment_id'];
		$payment_ref = $this->data['invoice_data']['payment_ref'];
		$InvoiceTotal = $this->data['invoice_total'];
    $Date = $this->data['invoice_date'];
		$Amount = isset($_GET['AMOUNT']) ? $_GET['AMOUNT'] : '';
		$Currency = isset($_GET['CURRENCY']) ? $_GET['CURRENCY'] : '';
		$Datetime = isset($_GET['DATETIME']) ? $_GET['DATETIME'] : '';
		?><table style="clear:both;width:100%;margin-top:30px;table-layout:fixed;">
			<tr>
				<td style="padding:5px 10px 5px 0px;font-size: 10pt;">
					<span class="pcs-label"><?php echo t('Invoice Number'); ?> :</span>
				</td>
				<td>
					<span id="tmp_payment_terms"><?php echo $invoice_id; ?></span>
				</td>
			</tr>
			<?php if($payment_id != '') : ?>
				<tr>
					<td style="padding:5px 10px 5px 0px;font-size: 10pt;">
						<span class="pcs-label"><?php echo t('Payment Id'); ?> :</span>
					</td>
					<td>
						<span id="tmp_payment_terms"><?php echo $payment_id; ?></span>
					</td>
				</tr>
			<?php endif;?>
			<tr>
				<td style="padding:5px 10px 5px 0px;font-size: 10pt;">
					<span class="pcs-label"><?php echo t('Payment Reference Number'); ?> :</span>
				</td>
				<td>
					<span id="tmp_payment_terms"><?php echo $payment_ref; ?></span>
				</td>
			</tr>
			<tr>
				<td style="padding:5px 10px 5px 0px;font-size: 10pt;">
					<span class="pcs-label"><?php echo t('Amount'); ?> :</span>
				</td>
				<td>
					<span id="tmp_payment_terms"><?php echo $InvoiceTotal; ?></span>
				</td>
			</tr>
			<tr>
				<td style="padding:5px 10px 5px 0px;font-size: 10pt;">
					<span class="pcs-label"><?php echo t('Date'); ?> :</span>
				</td>
				<td>
					<span id="tmp_payment_terms"><?php echo $Date; ?></span>
				</td>
			</tr>

		</table>
		<?php if($payment_id == '') : ?>
		
		<?php endif;
	}

	/**
	 * Get Invoice Data With HTML Structure
	 */
	public function quickbookInvoicePaymentHtml() {
		$invoice_data = $this->data['invoice_data'];
		global $base_url;
		$currency_code = $invoice_data['currency_code'];		
		?><div class="pcs-template ">
			<div class="pcs-template-body">
				<table style="clear:both;width:100%;margin-top:30px;table-layout:fixed;">
					<tr>
						<td style="padding:5px 10px 5px 0px;font-size: 10pt;">
							<span class="pcs-label"><?php echo t('Invoice Number'); ?> :</span>
						</td>
						<td>
							<span id="tmp_payment_terms"><?php echo $invoice_data['invoice_number']; ?></span>
						</td>
					</tr>
					<tr>  
						<td style="padding:5px 10px 5px 0px;font-size:10pt;">
							<span  class="pcs-label"><?php echo t('Invoice Date'); ?> :</span>
						</td>
						<td>
							<span id="tmp_entity_date"><?php echo $invoice_data['created_date']; ?></span>
						</td>
					</tr>
				</table>
				<table cellpadding="0" cellspacing="0" border="0" class="pcs-itemtable" style="width:100%;margin-top:20px;table-layout:fixed;">
					<thead>
						<tr style="height:32px;">
							<td class="pcs-itemtable-header pcs-itemtable-breakword" id="" style="padding: 5px 0px 5px 5px;width: 5%;text-align: center;">
							#
							</td>
							<td class="pcs-itemtable-header pcs-itemtable-breakword" id="" style="padding: 5px 10px 5px 20px;width: ;text-align: left;">
							<?php echo t('Item & Description'); ?>
							</td>
							<td class="pcs-itemtable-header pcs-itemtable-breakword" id="" style="padding: 5px 10px 5px 5px;width: 11%;text-align: right;">
							<?php echo t('Qty'); ?>
							</td>
							<td class="pcs-itemtable-header pcs-itemtable-breakword" id="" style="padding: 5px 10px 5px 5px;width: 11%;text-align: right;">
							<?php echo t('Unit Price'); ?>
							</td>
							<td class="pcs-itemtable-header pcs-itemtable-breakword" id="" style="padding: 5px 10px 5px 5px;width: 15%;text-align: right;">
							<?php echo t('Total Price'); ?>
							</td>
						</tr>
					</thead>
					<tbody class="itemBody">
						<?php foreach($invoice_data['line_item'] as $key => $line_item) { ?>
							<tr>
								<td class="pcs-item-row" style="padding: 10px 0 10px 5px;text-align: center;word-wrap: break-word;" valign="top"><?php echo $key+1; ?></td>
								<td class="pcs-item-row" style="padding: 10px 0px 10px 20px;" valign="top">
									<span id="tmp_item_name" style="word-wrap: break-word;"><?php echo $line_item['service']; ?></span>
								</td>
								<td class="pcs-item-row lineitem-column text-align-right">
									<div class="pcs-item-desc"><?php echo $line_item['quantity']; ?></div>
								</td>
								<td class="pcs-item-row lineitem-column text-align-right">
									<span id="tmp_item_rate"><?php echo $line_item['unit_price']; ?></span>
								</td>
								<td class="pcs-item-row lineitem-column lineitem-content-right text-align-right">
									<span id="tmp_item_amount"><?php echo $line_item['total_amount']; ?></span>
								</td>
							</tr>
						<?php } ?>	 
					</tbody>
				</table>
				<div style="width: 100%;margin-top: 1px;">
					<div class="v-top total-number-section">
						<div style="white-space: pre-wrap;"></div>
					</div>
					<div class="v-top total-section">
					<table width="100%" border="0"  cellspacing="0" class="pcs-totals">
						<tbody>
							<tr >
								<td class="total-section-label text-align-right"><b><?php echo t('Tax'); ?></b></td>
								<td class="total-section-value text-align-right" id="tmp_total"><b><?php echo $invoice_data['tax'] . ' ' . $currency_code; ?></b></td>               
							</tr>
							<tr >
								<td class="total-section-label text-align-right"><b><?php echo t('Total'); ?></b></td>
								<td class="total-section-value text-align-right" id="tmp_total"><b><?php echo $invoice_data['total'] . ' ' . $currency_code; ?></b></td>               
							</tr>
							<tr class="pcs-balance" style="height:40px;">
								<td class="total-section-label text-align-right"><b><?php echo t('Balance Due'); ?></b></td>
								<td class="total-section-value text-align-right" id="tmp_balance_due"><b><?php echo $invoice_data['balance'] . ' ' . $currency_code; ?></b></td>
							</tr>
						</tbody>
					</table>
				</div>
				<div style="clear: both;"></div>

				<?php if( $invoice_data['balance'] > 0 ) : 
					$url = ''; 
					if( variable_get('quickbook_invoice_menu_url', '') != '') {
						$url = variable_get('quickbook_invoice_menu_url', '') . '/';
					}					
					?>
					<!-- <a class="btn btn-danger pay-now" id="zoho-invoice-pay" target="_blank" href=" --><?php //echo $base_url . '/' . $url . 'payment?invoice_id=' . $invoice_data['invoice_number']; ?><!-- "> --><?php //echo t('Pay Now'); ?><!-- </a> -->
				<?php endif; ?>
			</div>
		</div><?php
	}


	/**
	 * Get Payment List With HTML Structure
	 */
	public function quickbookPaymentListHtml() {
		$payment_data = $this->data['invoice_data'];
		?>
		<div>
			<table class="table">
				<thead>
					<tr>
						<th>
							<?php echo t('Payment Id'); ?>
						</th>
						<th>
							<?php echo t('Date'); ?> 
						</th>
						<th>
							<?php echo t('Invoice Id'); ?> 
						</th>
						<th>
							<?php echo t('Customer Ref'); ?> 
						</th>
						<th>
							<?php echo t('Exchange Rate'); ?> 
						</th>
						<th>
							<?php echo t('Paid Amount'); ?> 
						</th>
						<th>
							<?php echo t('Currency Code'); ?> 
						</th>
					</tr>
				</thead>
				<tbody>
				<?php
				foreach ($payment_data as $Payment) {
					?><tr>
						<td>
							<?php echo $Payment['Id']; ?>
						</td>
						<td>
							<?php echo $Payment['TxnDate']; ?>
						</td>
						<td>
							<?php echo $Payment['invoice_id']; ?>
						</td>
						<td>
							<?php echo $Payment['CustomerRef']; ?>
						</td>
						<td>
							<?php echo $Payment['ExchangeRate']; ?>
						</td>
						<td>
							<?php echo $Payment['TotalAmt']; ?>
						</td>
						<td>
							<?php echo $Payment['CurrencyRef']; ?>
						</td>
					</tr><?php
				}?>
				</tbody>
			</table>
		</div><?php
	}




	/**
	 * Get Invoice List With HTML Structure
	 */
	public function quickbookInvoiceListHtml() {
		global $base_url;
		global $user;
		$user_account = user_load($user->uid);
		$invoice_data = $this->data['invoice_data'];
		$invoice_id = empty($_GET['Invoice-id'])? "" : $_GET['Invoice-id'];
		$customer_id = empty($_GET['Customer-id'])? "" : $_GET['Customer-id'];
		$customer_name = empty($_GET['Customer-Name'])? "" : $_GET['Customer-Name'];
		if(isset($_GET['Reset'])) {
			$invoice_id = "";
			$customer_id = "";
			$customer_name = "";
		}
		?>
		<div>
			<form class="form-inline" id="invoice-list-filter">
				<div class="form-group">
					<label for="first_name">Invoice Number</label>
					<input type="text" class="form-control" name="Invoice-id" value="<?php echo $invoice_id;  ?>">
				</div>
				<div class="form-group">
					<label for="last_name">Customer ID</label>
					<input type="text" class="form-control"  name="Customer-id" value="<?php echo $customer_id;  ?>">
				</div>
				<div class="form-group">
					<label for="last_name">Customer Name</label>
					<input type="text" class="form-control" name="Customer-Name" value="<?php echo $customer_name; ?>">
				</div>
				<input class="btn btn-primary" type="submit" name="search" value="Search">
			<form method="POST">
					<input class="btn btn-info"  type="submit" name="Reset" value="Reset">	
				</form>
			</form>
			<div class="table-responsive">
				<table class="table">
					<thead>
						<tr>
							<th>
								<?php echo t('Invoice Id'); ?>
							</th>
							<th>
								<?php echo t('Invoice Number'); ?> 
							</th>
							<th>
								<?php echo t('Customer Id'); ?> 
							</th>
							<th>
								<?php echo t('Customer Name'); ?> 
							</th>
							<th>
								<?php echo t('Date'); ?> 
							</th>
							<th>
								<?php echo t('User Email'); ?> 
							</th>
							<th>
								<?php echo t('Description'); ?> 
							</th>
							<th>
								<?php echo t('Qty'); ?> 
							</th>
							<th>
								<?php echo t('Total Amount'); ?> 
							</th>
							<th>
								<?php echo t('Due Balance'); ?> 
							</th>
							<th>
								<?php echo t('Currency Code'); ?> 
							</th>
							<th>
								<?php echo t('Due Date'); ?> 
							</th>
							<th>
								<?php echo t('Status'); ?> 
							</th>
							<th>
								<?php echo t('View Invoice'); ?> 
							</th>
							<th>
								<?php echo t('Payment'); ?> 
							</th>
						</tr>
					</thead>
					<tbody>
				<?php
				$count = 0;
				foreach ($invoice_data as $Invoice) {
					$pay_url = $base_url . "/quickbook/invoice/payment?invoice_id=" . $Invoice['invoice_number']; 
					$view_url = $base_url . "/quickbook/invoice/" . $user_account->field_user_company['und'][0]['value']."/payment/".$Invoice['invoice_number'];
					if($Invoice['due_balance'] > 0) {
						$Status = "Pending";
					}
					else {
						$Status = "Paid";
					}
					if(!isset($_GET['search'])) {
						$count = 1;
					?><tr>
						<td>
							<?php echo $Invoice['invoice_id']; ?>
						</td>
						<td>
							<?php echo $Invoice['invoice_number']; ?>
						</td>
						<td>
							<?php echo $Invoice['CustomerRef']; ?>
						</td>
						<td>
							<?php echo $Invoice['customer_name']; ?>
						</td>
						<td>
							<?php echo $Invoice['Date']; ?>
						</td>
						<td>
							<?php echo $Invoice['user_email']; ?>
						</td>
						<td>
							<?php echo $Invoice['Description']; ?>
						</td>
						<td>
							<?php echo $Invoice['Qty']; ?>
						</td>
						<td>
							<?php echo $Invoice['TotalAmt']; ?>
						</td>
						<td>
							<?php echo $Invoice['due_balance']; ?>
						</td>
						<td>
							<?php echo $Invoice['currency_code']; ?>
						</td>
						<td>
							<?php echo $Invoice['DueDate']; ?>
						</td>
						<td>
							<?php echo $Status; ?>
						</td>
						<td>
							<span><a class="btn btn-info" target="_blank" href="<?php echo $view_url; ?>"> View </a></span>
						</td>
						<td>
							<span><?php if($Invoice['due_balance']> 0) {echo '<b><a class="btn btn-danger pay-now" href="' . $pay_url . '"> Pay </a></b>';} else {echo 'Paid';}?> </span>
						</td>
					</tr><?php
					}else {
						$customer_name = false;
						if(!empty($_GET['Customer-Name'])) {
							$customer_name = stristr($Invoice['customer_name'], $_GET['Customer-Name']);
						}
						if($Invoice['invoice_number'] == $_GET['Invoice-id'] || $Invoice['CustomerRef'] == $_GET['Customer-id'] || $customer_name) {
							$count = $count + 1;
					?><tr>
						<td>
							<?php echo $Invoice['invoice_id']; ?>
						</td>
						<td>
							<?php echo $Invoice['invoice_number']; ?>
						</td>
						<td>
							<?php echo $Invoice['CustomerRef']; ?>
						</td>
						<td style="font-size: 11pt;width: 200px;text-align: left;border: 1.5px solid;">
							<?php echo $Invoice['customer_name']; ?>
						</td>
						<td>
							<?php echo $Invoice['Date']; ?>
						</td>
						<td style="font-size: 11pt;width: 100px;text-align: left;border: 1.5px solid;">
							<?php echo $Invoice['user_email']; ?>
						</td>
						<td style="font-size: 11pt;width: 100px;text-align: left;border: 1.5px solid;">
							<?php echo $Invoice['Description']; ?>
						</td>
						<td style="font-size: 11pt;width: 30px;text-align: center;border: 1.5px solid;">
							<?php echo $Invoice['Qty']; ?>
						</td>
						<td>
							<?php echo $Invoice['TotalAmt']; ?>
						</td>
						<td>
							<?php echo $Invoice['due_balance']; ?>
						</td>
						<td>
							<?php echo $Invoice['currency_code']; ?>
						</td>
						<td>
							<?php echo $Invoice['DueDate']; ?>
						</td>
						<td>
							<?php echo $Status; ?>
						</td>
						<td class="text-align-center" style="padding:5px 10px 5px 0px;font-size: 10pt;text-align: center;border: 1.5px solid;">
							<span class="pcs-label"><?php if($Invoice['due_balance'] > 0) {echo '<b><a target="_blanck" href="' . $pay_url . '"> Pay </a></b>';} else {echo 'Paid';}?> </span>
						</td>
					</tr>

					<?php } } }?>
				</tbody>
			</table>
			<?php if( $count == 0 ) { ?>
				<div><span> <strong>No Record Found..!</strong> </span> </div>
			<?php } ?>
		</div><?php 
	}
}