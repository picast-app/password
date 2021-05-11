#include "db.h"

DBClient::DBClient() {
  std::cout << "init api";
  Aws::InitAPI(sdk_opts);

  Aws::Client::ClientConfiguration clientConfig;
  clientConfig.connectTimeoutMs = 1000;
  clientConfig.requestTimeoutMs = 1000;
  clientConfig.region = "eu-west-1";
  auto credentialsProvider = Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>("LAMBDA_ALLOC");
  client = new Aws::DynamoDB::DynamoDBClient(credentialsProvider, clientConfig);
  std::cout << "init done";
};

DBClient::~DBClient() {
  Aws::ShutdownAPI(sdk_opts);
  delete client;
};

std::unique_ptr<Item> DBClient::getItem(std::string key) {
  Aws::DynamoDB::Model::GetItemRequest request;
  request.SetTableName("echo_passwords");
  request.AddKey("id", Aws::DynamoDB::Model::AttributeValue(key));

  auto result = client->GetItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
  const Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>& record = result.GetResult().GetItem();

  if (!record.size()) return nullptr;

  auto item = std::unique_ptr<Item>(new Item());
  item->id = record.at("id").GetS();
  item->hash = record.at("hash").GetS();
  return item;
};

void DBClient::putItem(std::string id, std::string hash) {
  Aws::DynamoDB::Model::PutItemRequest request;
  request.SetTableName("echo_passwords");

  request.AddItem("id", Aws::DynamoDB::Model::AttributeValue(id));
  request.AddItem("hash", Aws::DynamoDB::Model::AttributeValue(hash));

  auto result = client->PutItem(request);
  if (!result.IsSuccess()) throw result.GetError().GetMessage();
}
