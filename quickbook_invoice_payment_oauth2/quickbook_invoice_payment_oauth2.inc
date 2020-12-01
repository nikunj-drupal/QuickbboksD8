<?php 

$useroauth_mode = variable_get('quickbook_mode', 'development');
if ($useroauth_mode == 'development') {
require_once(drupal_get_path('module', 'quickbook_invoice_payment_oauth2') . "/libraries/Development/qbo/config.php");
}
elseif ($useroauth_mode == 'production') {
require_once(drupal_get_path('module', 'quickbook_invoice_payment_oauth2') . "/libraries/Production/qbo/config.php");
}
if (!$useroauth_mode) {
  require_once(drupal_get_path('module', 'quickbook_invoice_payment_oauth2') . "/libraries/Development/qbo/config.php");
}
// require_once(libraries_get_path('qbo') . '/config.php');
require_once(PATH_SDK_ROOT . 'Core/ServiceContext.php');
require_once(PATH_SDK_ROOT . 'DataService/DataService.php');
require_once(PATH_SDK_ROOT . 'PlatformService/PlatformService.php');
require_once(PATH_SDK_ROOT . 'Utility/Configuration/ConfigurationManager.php');
require_once PATH_SDK_ROOT . 'Data/IPPLine.php';
require_once PATH_SDK_ROOT . 'Data/IPPLinkedTxn.php';
require_once PATH_SDK_ROOT . 'Data/IPPPayment.php';
require_once PATH_SDK_ROOT . 'Facades/SalesReceipt.php';



/**
 * Quickbook Invoice API Request Handler
 */
class QuickBookInvoiceAPI {
    private $client_id;
    private $client_secret;
    protected $data = array();
    /**
	 * Service stype.
	 *
	 * @var string
	 */
	 public $serviceType;

	/**
	 * Realm Id.
	 *
	 * @var string
	 */
  public $realmId;

  /**
   * Access Token.
   *
   * @var string
   */
	public $accessToken;

	/**
	 * Request Validator.
	 *
	 * @var OAuthRequestValidator
	 */
  public $requestValidator;
	
	/**
	 * Service Context.
	 *
	 * @var ServiceContext
	 */
  public $serviceContext;

  /**
   * Oauth2 Logi Helper.
   *
   * @var OAuth2LoginHelper
   */
	public $OAuth2LoginHelper;

	/**
	 * Data Service.
	 *
	 * @var DataService
	 */
	public $dataService;
  /**
   * HTTP Methods
   */
  const HTTP_METHOD_GET    = 'GET';
  const HTTP_METHOD_POST   = 'POST';
  const HTTP_METHOD_PUT    = 'PUT';
  const HTTP_METHOD_DELETE = 'DELETE';
  const HTTP_METHOD_HEAD   = 'HEAD';
  const HTTP_METHOD_PATCH   = 'PATCH';

  public function __construct($client_id, $client_secret, $scope = null, $certificate_file = null) {	
  	global $base_url;
    if (!extension_loaded('curl')) {
        throw new Exception('The PHP exention curl must be installed to use this library.', Exception::CURL_NOT_FOUND);
    }
    if(!isset($client_id) || !isset($client_secret)){
        throw new Exception('The App key must be set.', Exception::InvalidArgumentException);
    }

    $this->client_id     = $client_id;
    $this->client_secret = $client_secret;
    $this->data['auth_mode'] = 'oauth2';
    $this->data['ClientID'] = $this->client_id;
    $this->data['ClientSecret'] = $this->client_secret;
    $this->data['RedirectURI'] = $base_url . "/oauth-redirect";
    $this->data['scope'] = $scope;
    $this->data['realm_id'] = variable_get('quickbook_company_id','');
    $this->dataService = DataService::Configure(array(
      'auth_mode' => $this->data['auth_mode'],
      'ClientID' => $this->data['ClientID'],
      'ClientSecret' => $this->data['ClientSecret'],
      'RedirectURI' => $this->data['RedirectURI'],
      'scope' => $this->data['scope'],
      'baseUrl' => "Development"
    ));
  }

 
  /**
 * @param $error_notify
 *	Connect To QucikBook Online
 */
  public function quickbookConnection( $error_notify ) {
      
      
      $OAuth2LoginHelper = $this->dataService->getOAuth2LoginHelper();

      $authorizationUrl = $OAuth2LoginHelper->getAuthorizationCodeURL();
     
      header("Location: ".$authorizationUrl);
      exit();
	}

//   public function setCertificate($certificate_file){
//     $this->certificate_file = $certificate_file;
//     if (!empty($this->certificate_file)  && !is_file($this->certificate_file)) {
//         throw new InvalidArgumentException('The certificate file was not found', InvalidArgumentException::CERTIFICATE_NOT_FOUND);
//     }
//   }

  /**
 * Re-Generate Token Before Expire
 * @return object
 */
  public function reCreateAccessTokens() {
    $dataService = DataService::Configure(array(
      'auth_mode' => $this->data['auth_mode'],
      'ClientID' => $this->data['ClientID'],
      'ClientSecret' => $this->data['ClientSecret'],
      // 'accessTokenKey' => variable_get('access_token',''),
      'refresh_tokenhTokenKey' => variable_get('refresh_token',''),
      'RedirectURI' => $this->data['RedirectURI'],
      'QBORealmID' => $this->data['realm_id'],
      'baseUrl' => "Development"
    ));
    $this->dataService = $dataService;
    $refresh_token = variable_get('refresh_token','');
    // $this->dataService->setLogLocation("/Users/hlu2/Desktop/newFolderForLog");
    $OAuth2LoginHelper = $this->dataService->getOAuth2LoginHelper();
    $accessToken = $OAuth2LoginHelper->refreshToken();
    $error = $OAuth2LoginHelper->getLastError();
    
    if ($error) {
        echo "The Status code is: " . $error->getHttpStatusCode() . "\n";
        echo "The Helper message is: " . $error->getOAuthHelperError() . "\n";
        echo "The Response message is: " . $error->getResponseBody() . "\n";
        return;
    }
    else {
        $this->dataService->updateOAuth2Token($accessToken);
    }
  }


  public function callForOpenIDEndpoint($access_token, $url,$realmID = ''){
    global $base_url;
    $authorizationHeaderInfo = $this->generateAccessTokenHeader($access_token);
    $http_header = array(
      'Accept' => 'application/json',
      'Authorization' => $authorizationHeaderInfo
    );
    $result = $this->executeRequest($url , null, $http_header, self::HTTP_METHOD_GET);
    $result['realmID'] = $realmID;
    return $result;
  }

  private function generateAccessTokenHeader($access_token){
    $authorizationheader = 'Bearer ' . $access_token;
    return $authorizationheader;
  }


  public function getAuthorizationURL($authorizationRequestUrl, $scope, $redirect_uri, $response_type, $state){
      $parameters = array(
        'client_id' => $this->client_id,
        'scope' => $scope,
        'redirect_uri' => $redirect_uri,
        'response_type' => $response_type,
        'state' => $state
        //The include_granted_scope is always set to false. No need to pass.
        //'include_granted_scope' => $include_granted_scope
      );
      $authorizationRequestUrl .= '?' . http_build_query($parameters, null, '&', PHP_QUERY_RFC1738);
      return $authorizationRequestUrl;
  }

  public function getAccessToken($tokenEndPointUrl, $code, $redirectUrl, $grant_type){
     if(!isset($grant_type)){
        throw new InvalidArgumentException('The grant_type is mandatory.', InvalidArgumentException::INVALID_GRANT_TYPE);
     }

     $parameters = array(
       'grant_type' => $grant_type,
       'code' => $code,
       'redirect_uri' => $redirectUrl
     );
     $authorizationHeaderInfo = $this->generateAuthorizationHeader();
     $http_header = array(
       'Accept' => 'application/json',
       'Authorization' => $authorizationHeaderInfo,
       'Content-Type' => 'application/x-www-form-urlencoded'
     );

     //Try catch???
     $result = $this->executeRequest($tokenEndPointUrl , $parameters, $http_header, self::HTTP_METHOD_POST);
     $this->accessToken = $result['access_token'];
     drupal_set_message($result['access_token']);
     if (user_is_anonymous() == 0) {
      global $user;
      $existingUser = user_load($user->uid);
      $existingUser -> field_quickbook_accestoken['und'][0]['value'] = $result['access_token'];
      user_save($existingUser);
     }
     $this->dataService->updateOAuth2Token($this->accessToken);
     return $result;
  }

  public function refreshAccessToken($tokenEndPointUrl, $grant_type, $refresh_token){
    $parameters = array(
      'grant_type' => $grant_type,
      'refresh_token' => $refresh_token
    );

    $authorizationHeaderInfo = $this->generateAuthorizationHeader();
    
    $http_header = array(
      'Accept' => 'application/json',
      'Authorization' => $authorizationHeaderInfo,
      'Content-Type' => 'application/x-www-form-urlencoded'
    );
    $result = $this->executeRequest($tokenEndPointUrl , $parameters, $http_header, self::HTTP_METHOD_POST);
    return $result;
  }

  private function generateAuthorizationHeader(){
      $encodedClientIDClientSecrets = base64_encode($this->client_id . ':' . $this->client_secret);
      $authorizationheader = 'Basic ' . $encodedClientIDClientSecrets;
      return $authorizationheader;
  }

  private function executeRequest($url, $parameters = array(), $http_header, $http_method)
  {

    $curl_options = array();

    switch($http_method){
          case self::HTTP_METHOD_GET:
            $curl_options[CURLOPT_HTTPGET] = 'true';
            if (is_array($parameters) && count($parameters) > 0) {
              $url .= '?' . http_build_query($parameters);
            } elseif ($parameters) {
              $url .= '?' . $parameters;
            }
            break;
          case self:: HTTP_METHOD_POST:
            $curl_options[CURLOPT_POST] = '1';
            if(is_array($parameters) && count($parameters) > 0){
              $body = http_build_query($parameters);
              $curl_options[CURLOPT_POSTFIELDS] = $body;
            }
            break;
          default:
            break;
    }
    /**
    * An array of HTTP header fields to set, in the format array('Content-type: text/plain', 'Content-length: 100')
    */
    if(is_array($http_header)){
          $header = array();
          foreach($http_header as $key => $value) {
              $header[] = "$key: $value";
          }
          $curl_options[CURLOPT_HTTPHEADER] = $header;
    }

    $curl_options[CURLOPT_URL] = $url;
    $ch = curl_init();

    curl_setopt_array($ch, $curl_options);
   
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

    //Execute the Curl Request
    $result = curl_exec($ch);

    $headerSent = curl_getinfo($ch, CURLINFO_HEADER_OUT );

    $http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);


    $content_type = curl_getinfo($ch, CURLINFO_CONTENT_TYPE);
     if ($curl_error = curl_error($ch)) {
         throw new Exception($curl_error);
     } else {
         $json_decode = json_decode($result, true);
     }
     curl_close($ch);

     //var_dump($json_decode);
     return $json_decode;
  }


  /**
	* @param $invoive (Invoice Doc Number)
	* Get QuickBook Invoice
	* @return array()
	*/
	public function getQuickBookInvoice( $invoice ) {

    if( empty( $this->settingErrorMessage ) ) {
			$sql = "SELECT * FROM Invoice WHERE DocNumber = '$invoice'";
      $invoiceData = $this->dataService->Query( $sql );
      $_SESSION["customer_email"] = (string)$invoiceData->Invoice->BillEmail->Address;
			if( !empty( $invoiceData[0] ) ) {
				return $this->getInvoiceFormatedData( $invoiceData[0] );
			}
			else {
				return [];
			}
		}
		else {
			return [];
		}
	}

  /**
   * @param $invoiceData
   * Format Invoice Data
   * @return array()
   */
  public function getInvoiceFormatedData( $invoiceData ) {
    $invoice = array();
   
    $billdata = (array) $invoiceData->Invoice;
 
    $invoice['line'] = $invoiceData->Invoice->Line;
    $invoice['invoice_id'] = $invoiceData->Invoice->Id;
    $invoice['invoice_number'] = $invoiceData->Invoice->DocNumber;
    $invoice['customer_ref'] = $invoiceData->Invoice->CustomerRef;
    list($invoice['created_date'], $invoice['created_time']) = explode( 'T', $invoiceData->Invoice->MetaData->CreateTime);
    $invoice['total'] = $invoiceData->Invoice->TotalAmt;
    $invoice['tax'] = $invoiceData->Invoice->TxnTaxDetail->Invoice->TotalTax;
    $invoice['currency_code'] = $invoiceData->Invoice->CurrencyRef;
    if(isset($billdata['BillEmail'])) {
      $invoice['user_email'] = $invoiceData->Invoice->BillEmail->Invoice->Address;
    }
    else {
      $invoice['user_email'] = "";
    }
    $invoice['balance'] = $invoiceData->Invoice->Balance;
    $invoice['status'] = $invoiceData->Invoice->status;

    if( count( $invoiceData->Invoice->Line ) > 0 ) {
      foreach ( $invoiceData->Invoice->Line as $lineItem ) {
        if( !empty( $lineItem->Id ) ) {
          $invoice['line_item'][] = [
            'service' => $lineItem->Description,
            'total_amount' => $lineItem->Amount,
            'unit_price' => $lineItem->SalesItemLineDetail->UnitPrice,
            'quantity' => $lineItem->SalesItemLineDetail->Qty,
          ];
        }
      }
    }
    return $invoice;
  }

  /**
   * @param $invoice 
   * Make Payment For Invoice (Invoice Doc Number)
   * @return object
   */
  public function createQuickBookInvoicePayment( $invoiceData ) {
    
    $TxnId = (array)$invoiceData['invoice_id'];
    $Amount = (array)$invoiceData['total'];
    $customer_ref = (array)$invoiceData['customer_ref'];

    
    if( !empty( $invoiceData ) ) {
      
      $qbLinkedInvoice = new IPPLinkedTxn();
      $qbLinkedInvoice->TxnId = $TxnId[0];
      $qbLinkedInvoice->TxnType = 'Invoice';

      $qbLine = new IPPLine();
      $qbLine->Amount = $Amount[0];
      $qbLine->LinkedTxn = $qbLinkedInvoice;

      $qbPayment = new IPPPayment();
      $qbPayment->CustomerRef = $customer_ref[0];
      $qbPayment->TotalAmt = $Amount[0];
      $qbPayment->Line = [$qbLine];

      $createdQbPayment = $this->dataService->Add( $qbPayment );
   
      return $createdQbPayment;
    }
  }


  /**
  * @param $invoive (Invoice Doc Number)
  * Get QuickBook Invoice
  * @return array()
  */
  public function getQuickBookInvoiceList() {
    
    if( empty( $this->settingErrorMessage ) ) {
      $Invoice = array();

      $sql = "SELECT * FROM Invoice";

      $invoiceData = $this->dataService->Query( $sql );
     
      $Invoice['invoiceData'] = $invoiceData;

      $sql = "SELECT Id, FullyQualifiedName FROM Customer";

      $customerData = $this->dataService->Query( $sql );
      
      $Invoice['customerData'] = $customerData;
  
      if( !empty( $invoiceData ) ) {
        return $this->getInvoiceFormatedList( $Invoice );
      }
      else {
        return [];
      }
    }
    else {
      return [];
    }
  }




  /**
  * @param $invoive (Invoice Doc Number)
  * Get QuickBook Invoice
  * @return array()
  */
  public function getQuickBookPaymentList() {
    
    if( empty( $this->settingErrorMessage ) ) {
      $sql = "SELECT * FROM Payment";
      $PaymentList = $this->dataService->Query( $sql );
    
      if( !empty( $PaymentList->Payment ) ) {
        return $this->getPaymentFormatedList( $PaymentList );
      }
      else {
        return [];
      }
    }
    else {
      return [];
    }
  }


  /**
   * @param $invoiceData
   * Format Invoice Data
   * @return array()
   */
  public function getInvoiceFormatedList( $invoiceData ) {
    // $invoiceData = (array) $invoiceData;
    $invoice = array();
    $invoiceList =(array) $invoiceData['invoiceData'];
    $customerList =(array) $invoiceData['customerData'];
   
    foreach ($invoiceList['Invoice'] as $key1 => $Invoice) {
      $Invoice = (array) $Invoice;
      foreach ($customerList['Customer'] as $key2 => $Customer) {
        $Customer = (array) $Customer;
        if($Invoice['CustomerRef'] == $Customer['Id']) {
          $invoice[$key1]['customer_name'] = $Customer['FullyQualifiedName'];
        }
      }
      if(isset($Invoice['BillEmail'])) {
        $user_email = (array) $Invoice['BillEmail'];
        $user_email = $user_email['Address'];
      }
      else {
        $$user_email = ""; 
      }
      
      $Line = (array) $Invoice['Line'][0];
      if (!empty($Line['SalesItemLineDetail'])) {
        $Qty = (array) $Line['SalesItemLineDetail'];
      }
      else{
        $Qty = '';
      }
     
      $invoice[$key1]['invoice_id'] = $Invoice['Id'];
      $invoice[$key1]['invoice_number'] = $Invoice['DocNumber'];
      $invoice[$key1]['Date'] = $Invoice['TxnDate'];
      $invoice[$key1]['user_email'] = $user_email;
      $invoice[$key1]['TotalAmt'] = $Invoice['TotalAmt'];
      if (!empty($Invoice['HomeTotalAmt'])) {
        $invoice[$key1]['HomeTotalAmt'] = $Invoice['HomeTotalAmt'];
      }
      else{
        $invoice[$key1]['HomeTotalAmt'] = '';
      }
      if (!empty($Invoice['ExchangeRate'])) {
        $invoice[$key1]['ExchangeRate'] = $Invoice['ExchangeRate'];
      }else{
        $invoice[$key1]['ExchangeRate'] = '';
      }
      $invoice[$key1]['due_balance'] = $Invoice['Balance'];
      $invoice[$key1]['currency_code'] = $Invoice['CurrencyRef'];
      $invoice[$key1]['DueDate'] = $Invoice['DueDate'];
      if (!empty($Line['Description'])) {
        $invoice[$key1]['Description'] = $Line['Description'];
      }
      else{
        $invoice[$key1]['Description'] = '';
      }
      if (!empty($Qty['Qty'])) {
        $invoice[$key1]['Qty'] = $Qty['Qty'];
      }
      else{
        $invoice[$key1]['Qty'] = '';
      }
      $invoice[$key1]['CustomerRef'] = $Invoice['CustomerRef'];
    }
    return $invoice;
  }



   /**
   * @param $paymentList
   * Format Payment Data
   * @return array()
   */
  public function getPaymentFormatedList( $paymentList ) {
    $paymentList = (array) $paymentList;
    $payemnt = array();
  
    foreach ($paymentList['Payment'] as $key => $Payment) {   
      $Payment = (array) $Payment;
      
      $invoice_number = (array)$Payment['Line']->LineEx->NameValue[2];
   
      $payemnt[$key]['Id'] = $Payment['Id'];
      $payemnt[$key]['TxnDate'] = $Payment['TxnDate'];
      $payemnt[$key]['invoice_id'] = $invoice_number['Value'];
      $payemnt[$key]['CustomerRef'] = $Payment['CustomerRef'];
      $payemnt[$key]['ExchangeRate'] = $Payment['ExchangeRate'];
      $payemnt[$key]['TotalAmt'] = $Payment['TotalAmt'];
      $payemnt[$key]['CurrencyRef'] = $Payment['CurrencyRef'];

    }
  
    return $payemnt;
  }
}