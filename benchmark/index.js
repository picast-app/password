const AWS = require('aws-sdk')
const region = 'eu-west-1'
const lambda = new AWS.Lambda({ region })
const fs = require('fs')
const path = require('path')

const KB = 1024
const MB = KB * 1024

const fileName = path.join(__dirname, `data/${new Date().toISOString()}.txt`)

main()
async function main() {
  const memStep = MB
  const params = { time: 1, memory: memStep, parallelism: 2 }

  fs.writeFileSync(
    fileName,
    `time: ${params.time}; parallelism: ${params.parallelism}\n`
  )

  while (params.memory <= MB * 175) {
    console.log(`${params.memory / MB} MiB`)
    const times = await runConfig(params)
    fs.appendFileSync(fileName, `\n${params.memory / KB},${times.join(',')}`)
    params.memory += memStep
  }
}

async function runConfig(params, iterations = 5) {
  let { memory, ...rest } = params
  memory /= KB
  return await Promise.all(
    [...Array(iterations)].map(() =>
      invoke({
        method: 'hash',
        password: 'foobar',
        memory,
        ...rest,
      }).then(({ durUS }) => {
        if (typeof durUS !== 'number') throw Error('no time received')
        return durUS
      })
    )
  )
}

async function invoke(payload) {
  const res = await lambda
    .invoke({ FunctionName: 'passwords', Payload: JSON.stringify(payload) })
    .promise()

  const data = JSON.parse(res.Payload)
  if (data.errorType) {
    console.warn(data.errorMessage)
    throw data.errorType
  }
  return data
}
